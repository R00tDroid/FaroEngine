#include "IToolchain.hpp"
#include "../ModuleManifest.hpp"
#include "../ToolchainInfo/ToolchainInfo.hpp"

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

    bool PrepareModuleForBuild(ModuleManifest& manifest, BuildPlatform* target) override
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

        objDir = GetObjDirectory(manifest, target);
        Utility::EnsureDirectory(objDir);

        return true;
    }

    bool BuildSource(ModuleManifest& manifest, BuildPlatform* target, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines) override
    {
        std::string includes = "";

        includes += " /I\"" + msvcRoot.string() + "\\include\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\shared\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\ucrt\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\winrt\"";
        includes += " /I\"" + windowsSdkInclude.string() + "\\um\"";

        for (std::filesystem::path& include : includePaths)
        {
            includes += " /I\"" + include.string() + "\"";
        }

        std::string defines = "";
        for (std::string& define : preprocessorDefines)
        {
            includes += " /D" + define;
        }

        std::filesystem::path clExe = msvcTools.string() + "\\cl.exe";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);
        std::filesystem::path outputFile = GetObjPath(manifest, target, sourceFile);
        Utility::EnsureDirectory(outputFile.parent_path());

        std::string log = "";
        int result = ExecuteCommand(msvcDrive.string() + ": & " + GetEnvCommand() + " & \"" + clExe.string() + "\" /c /FC /nologo /EHsc /Z7 /Od " + defines + " " + sourceFile.string() + " /Fo\"" + outputFile.string() + "\" " + includes, log);
        
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

    bool LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, std::vector<std::filesystem::path> sourceFiles) override
    {
        std::string objs = "";
        for (std::filesystem::path& sourceFile : sourceFiles)
        {
            objs += " \"" + GetObjPath(manifest, target, sourceFile).string() + "\"";
        }

        std::filesystem::path libExe = msvcTools / "lib.exe";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);

        std::filesystem::path libPath = GetBinPath(manifest, target);
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

    bool LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, std::vector<std::filesystem::path> sourceFiles) override
    {
        std::string libs = "";
        std::string libDirectories = "";
        std::string moduleLibs = "";

        libs += " \"user32.lib\"";

        //TODO switch debug and release
        libs += " \"libucrtd.lib\"";
        libs += " \"libvcruntimed.lib\"";

        //TODO Set correct architecture
        libDirectories += " /LIBPATH:\"" + windowsSdkLib.string() + "\\ucrt\\x64\"";
        libDirectories += " /LIBPATH:\"" + windowsSdkLib.string() + "\\um\\x64\"";
        libDirectories += " /LIBPATH:\"" + msvcRoot.string() + "\\lib\\x64\"";

        //TODO link against other modules (/WHOLEARCHIVE)
        //TODO link against dependencies

        std::filesystem::path linkExe = "\"" / msvcTools / "link.exe\"";
        std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);

        Utility::EnsureDirectory(GetBinDirectory(manifest));
        std::filesystem::path outputFile = GetBinPath(manifest, target);

        std::string log = "";
        int result = ExecuteCommand(msvcDrive.string() + ": & " + GetEnvCommand() + " & " + linkExe.string() + " /DEBUG /SUBSYSTEM:WINDOWS /MACHINE:X64 /OUT:\"" + outputFile.string() + "\" " + libs + libDirectories + moduleLibs, log);

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
