#pragma once
#include "ModuleManifest.hpp"

class ProjectManifest
{
public:
    std::filesystem::path manifestPath = "";
    std::filesystem::path projectDirectory = "";

    // Build directory of this project
    std::filesystem::path buildRoot = "";

    std::string projectName = "";
    std::vector<ModuleManifest> projectModules;

    bool Parse(std::filesystem::path path)
    {
        return true;
    }
};
