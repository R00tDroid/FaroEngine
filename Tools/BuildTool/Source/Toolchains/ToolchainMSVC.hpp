#include "IToolchain.hpp"
#include "MSVCInfo.hpp"
#include "WindowsKitInfo.hpp"
#include "../ModuleManifest.hpp"

enum EMSVCArchitecture 
{
    x64,
    x86
};

struct MSVCBuildPlatform : BuildPlatform
{
    EMSVCArchitecture Architecture;
};

class ToolchainMSVC : public IToolchain
{
public:
    static ToolchainMSVC Instance;

    std::vector<BuildPlatform*> GetPlatforms() override
    {
        if (platforms.empty())
        {
            platforms.push_back(new MSVCBuildPlatform{ "Windows x64", { "FARO_OS_WINDOWS", "FARO_ARCH_X64" }, {}, EMSVCArchitecture::x64 });
            platforms.push_back(new MSVCBuildPlatform{ "Windows x86", { "FARO_OS_WINDOWS", "FARO_ARCH_X86" }, {}, EMSVCArchitecture::x86 });
        }

        return platforms;
    }

    bool PrepareModuleForBuild(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration) override
    {
        MSVCBuildPlatform* buildPlatform = reinterpret_cast<MSVCBuildPlatform*>(target);

        const std::vector<WindowsKit>& windowsKits = GetWindowsKits();
        if (!windowsKits.empty())
        {
            std::string root = windowsKits[0].Root;
            std::string version = windowsKits[0].Version;

            windowsSdkInclude = root + "\\Include\\" + version;
            windowsSdkLib = root + "\\Lib\\" + version;
        }

        const std::vector<MSVCVersion>& MSVCInstallations = GetMSVCInstallations();
        if (!MSVCInstallations.empty())
        {
            msvcRoot = MSVCInstallations[0].Root;
        }

        if (windowsSdkLib == "" || windowsSdkInclude == "" || !std::filesystem::exists(windowsSdkLib) || !std::filesystem::exists(windowsSdkInclude))
        {
            Utility::PrintLine("Invalid WindowsKit directory");
            return false;
        }

        if (msvcRoot == "" || !std::filesystem::exists(msvcRoot))
        {
            Utility::PrintLine("Invalid MSVC directory");
            return false;
        }

        msvcTools = msvcRoot / "bin";
        switch (buildPlatform->Architecture)
        {
            case EMSVCArchitecture::x64:
            {
                msvcTools /= "Hostx64\\x64";
                break;
            }
            case EMSVCArchitecture::x86:
            {
                msvcTools /= "Hostx86\\x86";
                break;
            }
        }

        if (!std::filesystem::exists(windowsSdkInclude) || !std::filesystem::exists(windowsSdkLib) || !std::filesystem::exists(msvcTools))
        {
            Utility::PrintLine("Invalid WindowsKit");
            return false;
        }

        objDir = GetObjDirectory(manifest, target, configuration);
        Utility::EnsureDirectory(objDir);

        return true;
    }

    bool BuildSource(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines) override
    {
        std::string includes = "";

        includes += " /I\"" + msvcRoot.string() + "\\include\""; //TODO Don't define here but expose to ensure they're in-line with the generated project
        includes += " /I\"" + windowsSdkInclude.string() + "\\shared\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\ucrt\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\winrt\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\um\"";

        for (std::filesystem::path& include : includePaths)
        {
            includes += " /I\"" + include.string() + "\"";
        }

        std::string defines;
        for (std::string& define : preprocessorDefines)
        {
            defines += " /D" + define;
        }

        std::string compilerFlags;
        switch (configuration)
        {
            case Debug:
            case Development: { compilerFlags = " /DDEBUG /D_DEBUG /D_MT /D_CRTDBG_MAP_ALLOC /MTd /Od /Zi"; break; }
            case Release: { compilerFlags = " /D_MT /D_CRTDBG_MAP_ALLOC /MT /O2"; break; }

            case ENUMSIZE:;
            default:;
        }

        std::filesystem::path clExe = msvcTools.string() + "\\cl.exe";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);
        std::filesystem::path outputFile = GetObjPath(manifest, target, configuration, sourceFile);
        Utility::EnsureDirectory(outputFile.parent_path());

        std::string log = "";
        int result = ExecuteCommand(msvcDrive.string() + ": & " + GetEnvCommand() + " & \"" + clExe.string() + "\" /c /FC /nologo /EHsc " + defines + " " + compilerFlags + " " + sourceFile.string() + " /Fo\"" + outputFile.string() + "\" " + includes, log);
        
        //Format, trim and print output message
        if (!log.empty())
        {
            log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

            std::string header = sourceFile.filename().string() + "\n";
            if (log.substr(0, header.size()) == header)
            {
                log = log.substr(header.size());
            }

            if (!log.empty())
            {
                Utility::PrintLine(log);
            }
        }

        return result == 0;
    }

    bool LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) override
    {
        std::string objs = "";
        for (std::filesystem::path& sourceFile : sourceFiles)
        {
            objs += " \"" + GetObjPath(manifest, target, configuration, sourceFile).string() + "\"";
        }

        std::filesystem::path libExe = msvcTools / "lib.exe";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);

        std::filesystem::path libPath = GetLibPath(manifest, target, configuration);
        Utility::EnsureDirectory(libPath.parent_path());

        std::string log = "";
        int result = ExecuteCommand(msvcDrive.string() + ": & " + GetEnvCommand() + " & \"" + libExe.string() + "\" /nologo /OUT:\"" + libPath.string() + "\" " + objs, log);

        //Format, trim and print output message
        if (!log.empty())
        {
            log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

            if (!log.empty())
            {
                Utility::PrintLine(log);
            }
        }

        return result == 0;
    }

    bool LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) override
    {
        std::string objs = "";
        for (std::filesystem::path& sourceFile : sourceFiles)
        {
            objs += " \"" + GetObjPath(manifest, target, configuration, sourceFile).string() + "\"";
        }

        std::string libs = "";
        std::string libDirectories = "";
        std::string moduleLibs = "";

        libs += " \"user32.lib\"";

        switch (configuration)
        {
            case Debug:
            case Development:
            {
                libs += " \"libucrtd.lib\"";
                libs += " \"libvcruntimed.lib\"";
                break;
            }
            case Release: 
            {
                libs += " \"libucrt.lib\"";
                libs += " \"libvcruntime.lib\"";
                break;
            }

            case ENUMSIZE:;
            default:;
        }

        //TODO Set correct architecture
        libDirectories += " /LIBPATH:\"" + windowsSdkLib.string() + "\\ucrt\\x64\"";
        libDirectories += " /LIBPATH:\"" + windowsSdkLib.string() + "\\um\\x64\"";
        libDirectories += " /LIBPATH:\"" + msvcRoot.string() + "\\lib\\x64\"";

        for (ModuleManifest* module : manifest.moduleDependencies)
        {
            std::filesystem::path lib = GetLibPath(*module, target, configuration);
            Utility::PrintLineD("\t" + lib.string());
            moduleLibs += " /WHOLEARCHIVE:\""+ lib.string() + "\"";

            for (std::string& linkerLibrary : module->linkingLibraries)
            {
                libs += " \"" + linkerLibrary + "\"";
            }
        }

        std::filesystem::path linkExe = msvcTools / "link.exe";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);

        std::filesystem::path outputFile = GetExePath(manifest, target, configuration);
        Utility::EnsureDirectory(outputFile.parent_path());

        std::string flags = "/NOLOGO /SUBSYSTEM:CONSOLE /WX /MACHINE:X64";

        if (configuration == Debug || configuration == Development)
        {
            flags += " /DEBUG";
        }

        std::string log = "";
        int result = ExecuteCommand(msvcDrive.string() + ": & " + GetEnvCommand() + " & \"" + linkExe.string() + "\" " + flags + " /OUT:\"" + outputFile.string() + "\" " + objs + libs + libDirectories + moduleLibs, log);

        //Format, trim and print output message
        if (!log.empty())
        {
            log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

            if (!log.empty())
            {
                Utility::PrintLine(log);
            }
        }
        return result == 0;
    }

    std::string GetObjExtension() override
    {
        return "obj";
    }

    std::string GetLibExtension() override
    {
        return "lib";
    }

    std::string GetExeExtension() override
    {
        return "exe";
    }

private:
    std::string GetEnvCommand()
    {
        //TODO add MSVC IDE path
        return "set \"path=C:\\Windows\\System32;\"";
    }
    std::filesystem::path objDir = "";

    std::filesystem::path msvcRoot = "";
    std::filesystem::path msvcTools = "";

    std::filesystem::path windowsSdkInclude = "";
    std::filesystem::path windowsSdkLib = "";

    std::vector<BuildPlatform*> platforms;
};

inline ToolchainMSVC ToolchainMSVC::Instance;
