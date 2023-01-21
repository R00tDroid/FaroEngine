#include "IToolchain.hpp"
#include "../ModuleManifest.hpp"
#include "../ProjectManifest.hpp"
#include "Command.hpp"
#include "ToolchainMSVC.hpp"

std::vector<std::string> IToolchain::GetPreprocessorDefines(BuildPlatform* platform, BuildType configuration)
{
    std::vector<std::string> defines;

    for (std::string& define : platform->preprocessorDefines)
    {
        defines.push_back(define);
    }

    switch (configuration)
    {
        case Debug: { defines.push_back("FARO_DEBUG"); break; }
        case Development: { defines.push_back("FARO_DEVELOPMENT"); break; }
        case Release: { defines.push_back("FARO_RELEASE"); break; }
    }

    return defines;
}

int IToolchain::ExecuteCommand(std::string command, std::string& output)
{
    return Utility::ExecuteCommand(command, output);
}

int IToolchain::ExecuteCommand(std::string command)
{
    std::string discardLog = "";
    return ExecuteCommand(command, discardLog);
}

std::vector<IToolchain*> IToolchain::GetToolchains()
{
    return { &ToolchainMSVC::Instance };
}

std::filesystem::path IToolchain::GetObjDirectory(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');
    return manifest.faroRoot / "Obj" / platformName;
}

std::filesystem::path IToolchain::GetObjPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile)
{
    return GetObjDirectory(manifest, target, configuration) / sourceFile.filename().replace_extension(GetObjExtension());
}

std::filesystem::path IToolchain::GetLibDirectory(ModuleManifest& manifest)
{
    return manifest.faroRoot / "Bin";
}

std::filesystem::path IToolchain::GetLibPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');

    return GetLibDirectory(manifest) / (platformName + "." + GetLibExtension());
}

std::filesystem::path IToolchain::GetExeDirectory(ProjectManifest& manifest)
{
    return manifest.faroRoot / "Bin";
}

std::filesystem::path IToolchain::GetExePath(ProjectManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');

    return GetExeDirectory(manifest) / (manifest.projectName + "_" + platformName + "." + GetExeExtension());
}
