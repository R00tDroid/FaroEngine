#pragma once

class ProjectManifest;

class ModuleManifest
{
public:
    inline static std::string moduleFileSuffix = ".module.json";

    // Name of the module
    std::string name = "";

    // Full path to module manifest
    std::string manifestPath = "";

    // Root directory of this module
    std::string moduleRoot = "";

    // Build directory of this module
    std::string buildRoot = "";

    // Project this module belongs to
    ProjectManifest* project = nullptr;

    // List of modules this module depends on
    std::vector<ModuleManifest*> moduleDependencies;

    std::vector<std::string> sourceFiles;
};
