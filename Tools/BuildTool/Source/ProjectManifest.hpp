#pragma once
#include "ModuleManifest.hpp"
#include <picojson.h>
#include <fstream>

class ProjectManifest
{
public:
    inline static std::string projectFileSuffix = ".faroproject.json";

    std::filesystem::path manifestPath = "";
    std::filesystem::path projectDirectory = "";

    // Build directory of this project
    std::filesystem::path buildRoot = "";

    std::string projectName = "";
    std::vector<ModuleManifest> projectModules;

    bool Parse(std::filesystem::path path)
    {
        std::ifstream fileStream(path);
        if (!fileStream.is_open())
        {
            Utility::PrintLine("Failed to open project file: " + path.string());
            return false;
        }

        picojson::value rootObject;
        std::string error = picojson::parse(rootObject, fileStream);
        if (!error.empty())
        {
            Utility::Print("JSON parsing error: " + error);
            return false;
        }

        return true;
    }
};
