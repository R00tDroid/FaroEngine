#include "IToolchain.hpp"
#include "../ModuleManifest.hpp"
#include <array>
#include "ToolchainMSVC.hpp"

int IToolchain::ExecuteCommand(std::string command, std::string& output)
{
    std::array<char, 16> logBuffer {};
    FILE* processPipe = _popen((command + " 2>&1").c_str(), "r");
    if (!processPipe)
    {
        return -1;
    }
    while (fgets(logBuffer.data(), logBuffer.size(), processPipe) != nullptr)
    {
        output += logBuffer.data();
    }

    return _pclose(processPipe);
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

std::filesystem::path IToolchain::GetObjDirectory(ModuleManifest& manifest, BuildPlatform* target)
{
    std::string platformName = target->platformName;
    std::transform(platformName.begin(), platformName.end(), platformName.begin(), tolower);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');
    return manifest.faroRoot / "Obj" / platformName;
}

std::filesystem::path IToolchain::GetObjPath(ModuleManifest& manifest, BuildPlatform* target, std::filesystem::path sourceFile)
{
    return GetObjDirectory(manifest, target) / sourceFile.parent_path() / (sourceFile.stem().string() + "." + GetObjExtension());
}

std::filesystem::path IToolchain::GetBinDirectory(ModuleManifest& manifest)
{
    return manifest.faroRoot / "Bin";
}

std::filesystem::path IToolchain::GetBinPath(ModuleManifest& manifest, BuildPlatform* target)
{
    std::string platformName = target->platformName;
    std::transform(platformName.begin(), platformName.end(), platformName.begin(), tolower);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');

    if (true) //TODO switch for libraries and executables
    {
        return GetBinDirectory(manifest) / (platformName + "." + GetLibExtension());
    }
    else
    {
        return GetBinDirectory(manifest) / (manifest.name + "_" + platformName + "." + GetExeExtension());
    }
}
