#include "ToolchainMSVC.hpp"
#include "Command.hpp"
#include "Utility.hpp"

ToolchainMSVC msvcInstance;

TargetMSVC::TargetMSVC(const char* configName, const char* configId, ToolchainMSVC* msvcToolchain): configName(configName), configId(configId), msvcToolchain(msvcToolchain) {}

const char* TargetMSVC::platform() const { return "windows"; }

const char* TargetMSVC::displayName() const { return configName; }

const char* TargetMSVC::identifier() const { return configId; }

const Toolchain* TargetMSVC::toolchain() const { return msvcToolchain; }

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
    }
    else
    {
        msvcVersion = msvcInstallation(0);
        configurations.push_back(TargetMSVC("Windows", "windowsx64", this));
    }
}

unsigned int ToolchainMSVC::targets()
{
    return static_cast<unsigned int>(configurations.size());
}

Target* ToolchainMSVC::target(unsigned int index)
{
    return &configurations[index];
}

bool ToolchainMSVC::compile(const ToolchainCompileInfo& info) const
{
    std::filesystem::path sourceFile(info.file);
    std::filesystem::path msvcRoot = msvcVersion.root;
    std::filesystem::path msvcTools = msvcRoot / "bin" / "Hostx64" / "x64";

    std::string includes = "";

    includes += " /I\"" + msvcRoot.string() + "\\include\""; //TODO Don't define here but expose to ensure they're in-line with the generated project
    /*includes += " /I\"" + windowsSdkInclude.string() + "\\shared\"";
    includes += " /I\"" + windowsSdkInclude.string() + "\\ucrt\"";
    includes += " /I\"" + windowsSdkInclude.string() + "\\winrt\"";
    includes += " /I\"" + windowsSdkInclude.string() + "\\um\"";*/

    for (unsigned i = 0; i < info.includePaths; i++)
    {
        const char* path = info.includePathsPtr[i];
        includes += " /I\"" + std::string(path) + "\"";
    }

    std::string defines;
    for (unsigned i = 0; i < info.defines; i++)
    {
        const char* define = info.definesPtr[i];
        includes += " /D" + std::string(define);
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

    std::filesystem::path clExe = msvcTools.string() + "\\cl.exe";
    std::filesystem::path msvcDrive = msvcRoot.string().substr(0, 1);
    std::filesystem::path outputFile = info.output;
    Utility::EnsureDirectory(outputFile.parent_path().string().c_str());

    //TODO Fix pdb path, and check if we need /FS
    std::string log = "";
    int result = Utility::ExecuteCommand(msvcDrive.string() + ": & \"" + clExe.string() + "\" /c /FC /FS /nologo /EHsc " + defines + " " + compilerFlags + " " + sourceFile.string() + " /Fo\"" + outputFile.string() + "\" " + includes, log);

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
