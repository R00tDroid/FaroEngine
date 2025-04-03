#include "ToolchainMSVC.hpp"
#include "Command.hpp"
#include "Utility.hpp"
#include "WindowsKitInfo/WindowsKitInfo.hpp"

ToolchainMSVC msvcInstance;

std::vector<const char*> TargetMSVC::msvcDefines = {
    "FARO_OS_WINDOWS"
};

TargetMSVC::TargetMSVC(const char* configName, const char* configId, ToolchainMSVC* msvcToolchain): configName(configName), configId(configId), msvcToolchain(msvcToolchain) {}

const char* TargetMSVC::displayName() const { return configName; }

const char* TargetMSVC::identifier() const { return configId; }

Toolchain* TargetMSVC::toolchain() const { return msvcToolchain; }

unsigned int TargetMSVC::defines() const
{
    return static_cast<unsigned int>(msvcDefines.size());
}

const char* TargetMSVC::define(unsigned int index) const
{
    return msvcDefines[index];
}

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}

ToolchainMSVC::ToolchainMSVC()
{
    unsigned int msvcVersions = msvcInstallations();
    if (msvcVersions == 0)
    {
        Utility::PrintLine("MSVC not found. Toolchain not available");
        return;
    }
    msvcVersion = msvcInstallation(0);

    unsigned int windowsKitVersions = windowsKits();
    if (windowsKitVersions == 0)
    {
        Utility::PrintLine("Windows Kit not found. Toolchain not available");
        return;
    }
    WindowsKit kit = windowsKit(0);
    std::filesystem::path root = kit.root;
    std::string version = kit.version;

    windowsSdkInclude = root / "Include" / version;
    windowsSdkLib = root / "Lib" / version;
    windowsUmLib = root / "Lib" / version / "um" / "x64";
    windowsUcrtLib = root / "Lib" / version / "ucrt" / "x64";

    configurations.push_back(TargetMSVC("Windows", "windowsx64", this));
}

unsigned int ToolchainMSVC::targets()
{
    return static_cast<unsigned int>(configurations.size());
}

Target* ToolchainMSVC::target(unsigned int index)
{
    return &configurations[index];
}

bool ToolchainMSVC::prepare(const BuildSetup&)
{
    msvcRoot = msvcVersion.root;
    msvcTools = msvcRoot / "bin" / "Hostx64" / "x64";
    msvcLib = msvcRoot / "lib" / "x64";

    clExe = msvcTools.string() + "\\cl.exe";
    libExe = msvcTools.string() + "\\lib.exe";
    linkExe = msvcTools.string() + "\\link.exe";
    msvcDrive = msvcRoot.string().substr(0, 1);

    includePaths = {
        (msvcRoot / "include").string(),
        (windowsSdkInclude / "shared").string(),
        (windowsSdkInclude / "ucrt").string(),
        (windowsSdkInclude / "winrt").string(),
        (windowsSdkInclude / "um").string(),
    };

    defines = {
        //TODO Set toolchain defines
    };

    return true;
}

bool ToolchainMSVC::compile(const ToolchainCompileInfo& info) const
{
    std::filesystem::path sourceFile(info.file);

    std::string includesString = "";
    for (const std::string& include : includePaths)
    {
        includesString += " /I\"" + include + "\"";
    }
    for (unsigned i = 0; i < info.includePaths; i++)
    {
        const char* path = info.includePathsPtr[i];
        includesString += " /I\"" + std::string(path) + "\"";
    }

    std::string definesString;
    for (const std::string& define : defines)
    {
        definesString += " /D" + define;
    }
    for (unsigned i = 0; i < info.defines; i++)
    {
        const char* define = info.definesPtr[i];
        definesString += " /D" + std::string(define);
    }

    std::string compilerFlags;
    switch (info.buildSetup.configuration)
    {
        case C_Debug:
        case C_Development: { compilerFlags = " /DDEBUG /D_DEBUG /D_MT /D_CRTDBG_MAP_ALLOC /MTd /Od /Zi"; break; }
        case C_Release: { compilerFlags = " /D_MT /D_CRTDBG_MAP_ALLOC /MT /O2"; break; }

        case C_ENUMSIZE:;
        default:;
    }

    std::filesystem::path outputFile = info.output;
    Utility::EnsureDirectory(outputFile.parent_path().string().c_str());

    //TODO Fix pdb path, and check if we need /FS
    std::string log = "";
    int result = Utility::ExecuteCommand(msvcDrive.string() + ": & \"" + clExe.string() + "\" /c /FC /FS /nologo /EHsc " + definesString + " " + compilerFlags + " " + sourceFile.string() + " /Fo\"" + outputFile.string() + "\" " + includesString, log);

    // Format, trim and print output message
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
            info.onLog(info.userData, static_cast<unsigned int>(log.length()), log.c_str());
        }
    }

    return result == 0;
}

bool ToolchainMSVC::link(const ToolchainLinkInfo& info) const
{
    std::string objFiles = "";
    for (unsigned i = 0; i < info.objFiles; i++)
    {
        const char* path = info.objFilesPtr[i];
        objFiles += " \"" + std::string(path) + "\"";
    }

    std::string libs = "";
    for (unsigned i = 0; i < info.linkLibs; i++)
    {
        const char* path = info.linkLibsPtr[i];
        libs += " \"" + std::string(path) + "\""; //TODO Should be wholearchive?
    }

    std::string libDirs;
    libDirs += " /LIBPATH:\"" + msvcLib.string() + "\""; //TODO Expose this from the toolchain
    libDirs += " /LIBPATH:\"" + windowsSdkLib.string() + "\"";
    libDirs += " /LIBPATH:\"" + windowsUmLib.string() + "\"";
    libDirs += " /LIBPATH:\"" + windowsUcrtLib.string() + "\"";

    for (unsigned i = 0; i < info.libPaths; i++)
    {
        const char* path = info.libPathsPtr[i];
        libDirs += " /LIBPATH:\"" + std::string(path) + "\"";
    }

    std::filesystem::path outputFile = info.output;
    Utility::EnsureDirectory(outputFile.parent_path().string().c_str());

    std::string flags = "/NOLOGO /SUBSYSTEM:CONSOLE /WX /MACHINE:X64";

    std::string log = "";
    int result = 0;

    switch (info.linkType) {
        case LT_StaticLibrary:
        {
            result = Utility::ExecuteCommand(msvcDrive.string() + ": & \"" + libExe.string() + "\" /nologo /OUT:\"" + outputFile.string() + "\" " + objFiles, log);
            break;
        }
        case LT_Application:
        {
            //TODO Link against modules
            result = Utility::ExecuteCommand(msvcDrive.string() + ": & \"" + linkExe.string() + "\" " + flags + " /OUT:\"" + outputFile.string() + "\" " + libDirs + " " + libs + " " + objFiles, log);
            break;
        }
    }

    // Format, trim and print output message
    if (!log.empty())
    {
        log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

        if (!log.empty())
        {
            info.onLog(info.userData, static_cast<unsigned int>(log.length()), log.c_str());
        }
    }

    return result == 0;
}

char* ToolchainMSVC::getBinExtension() const
{
    return ".obj";
}

char* ToolchainMSVC::getLinkExtension(LinkType type) const
{
    switch (type)
    {
    case LT_StaticLibrary:
        return ".lib";
    case LT_Application:
        return ".exe";
    }
    return nullptr;
}
