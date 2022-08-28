#pragma once
#include <filesystem>

class ProjectManifest;

class ModuleManifest
{
public:
    inline static std::string moduleFileSuffix = ".module.json";

    // Name of the module
    std::string name = "";

    // Full path to module manifest
    std::filesystem::path manifestPath = "";

    // Root directory of this module
    std::filesystem::path moduleRoot = "";

    // Build directory of this module
    std::filesystem::path buildRoot = "";

    // Project this module belongs to
    ProjectManifest* project = nullptr;

    // List of modules this module depends on
    std::vector<ModuleManifest*> moduleDependencies;

    std::vector<std::filesystem::path> sourceFiles;

    void Load()
    {
        moduleRoot = manifestPath.parent_path();
        name = manifestPath.filename().string();
        name.erase(name.length() - moduleFileSuffix.length());

        buildRoot = moduleRoot / ".Faro";
    }

    void Save()
    {
        
    }
};
