#pragma once
#include "ModuleManifest.hpp"

class ProjectManifest
{
public:
    std::string manifestPath = "";
    std::string projectDirectory = "";

    // Build directory of this project
    std::string buildRoot = "";

    std::string projectName = "";
    std::vector<ModuleManifest> projectModules;

    bool Parse(std::string path)
    {
        return true;
    }
};
