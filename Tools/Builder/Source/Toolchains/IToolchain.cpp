#include "IToolchain.hpp"
#include <Utility.hpp>
#include <Manifests/ModuleManifest.hpp>
#include <Manifests/ProjectManifest.hpp>
#include "Command.hpp"
#include "ToolchainMSVC.hpp"
#include "ToolchainEmscripten.hpp"

std::vector<std::string> IToolchain::GetPreprocessorDefines(ModuleManifest* manifest, BuildPlatform* platform, BuildType configuration)
{
    std::vector<std::string> defines;

    std::set<std::string> platformDefines;
    for (IToolchain* toolchain : IToolchain::GetToolchains())
    {
        for (BuildPlatform* toolchainPlatform : toolchain->GetPlatforms())
        {
            for (std::string& define : toolchainPlatform->preprocessorDefines)
            {
                platformDefines.insert(define);
            }
        }
    }

    for (std::string define : platformDefines)
    {
        bool found = std::find(platform->preprocessorDefines.begin(), platform->preprocessorDefines.end(), define) != platform->preprocessorDefines.end();
        defines.push_back(define + "=" + (found ? '1' : '0'));
    }

    defines.push_back(std::string("FARO_DEBUG=") + (configuration == Debug ? '1' : '0'));
    defines.push_back(std::string("FARO_DEVELOPMENT=") + (configuration == Development ? '1' : '0'));
    defines.push_back(std::string("FARO_RELEASE=") + (configuration == Release ? '1' : '0'));

    if (manifest != nullptr)
    {
        std::vector<std::string> moduleDefines = manifest->GetModuleDefines();
        for (std::string& define : moduleDefines)
        {
            defines.push_back(define);
        }
    }

    return defines;
}

std::set<ModuleManifest*> IToolchain::GetAllModuleDependencies(ModuleManifest& topModule)
{
    std::vector<std::filesystem::path> toProcess = { topModule.moduleDependencies };
    std::set<ModuleManifest*> dependencies;

    while (!toProcess.empty())
    {
        std::filesystem::path dependencyPath = toProcess[0];
        toProcess.erase(toProcess.begin());

        ModuleManifest* dependency = ModuleManifest::GetLoadedModule(dependencyPath);

        if (dependencies.find(dependency) == dependencies.end())
        {
            dependencies.insert(dependency);
            for (std::filesystem::path childDependencyPath : dependency->moduleDependencies)
            {
                toProcess.push_back(childDependencyPath);
            }
        }
    }

    return dependencies;
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
    return { &ToolchainMSVC::Instance, &ToolchainEmscripten::Instance };
}

std::filesystem::path IToolchain::GetObjDirectory(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');
    return manifest.faroDirectory / "Obj" / platformName;
}

std::filesystem::path IToolchain::GetObjPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile)
{
    return GetObjDirectory(manifest, target, configuration) / sourceFile.filename().replace_extension(GetObjExtension());
}

std::filesystem::path IToolchain::GetBinDirectory(ModuleManifest& manifest)
{
    return manifest.faroDirectory / "Bin";
}

std::filesystem::path IToolchain::GetLibPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');

    return GetBinDirectory(manifest) / (platformName + "." + GetLibExtension());
}

std::filesystem::path IToolchain::GetExePath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    std::string platformName = target->platformName + " " + BuildTypeNames[configuration];
    platformName = Utility::ToLower(platformName);
    std::replace(platformName.begin(), platformName.end(), ' ', '_');

    return GetBinDirectory(manifest) / (manifest.name + "_" + platformName + "." + GetExeExtension());
}

std::filesystem::path IToolchain::GetBinaryPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration)
{
    if (manifest.type == MT_Library)
    {
        return GetLibPath(manifest, target, configuration);
    }
    else if (manifest.type == MT_Executable)
    {
        return GetExePath(manifest, target, configuration);
    }
    return {};
}
