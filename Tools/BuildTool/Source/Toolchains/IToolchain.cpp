#include "IToolchain.hpp"
#include "../ModuleManifest.hpp"
#include "../ProjectManifest.hpp"
#include <array>
#include "Command.hpp"
#include "ToolchainMSVC.hpp"

#ifdef WIN32
#define popen_impl _popen
#define pclose_impl _pclose
#else
#define popen_impl popen
#define pclose_impl pclose
#endif

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
