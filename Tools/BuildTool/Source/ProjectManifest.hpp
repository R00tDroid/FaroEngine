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

        picojson::value rootValue;
        std::string error = picojson::parse(rootValue, fileStream);
        if (!error.empty())
        {
            Utility::Print("JSON parsing error: " + error);
            return false;
        }

        if (!rootValue.is<picojson::object>())
        {
            Utility::PrintLine("Expected a JSON object as root of the project manifest");
            return false;
        }

        picojson::object& rootObject = rootValue.get<picojson::object>();

        if (!ParseProject(rootObject)) return false;
        if (!ParseModules(rootObject)) return false;

        return true;
    }

    bool ParseProject(picojson::object& rootObject)
    {
        if (rootObject.find("Name") != rootObject.end())
        {
            picojson::value& value = rootObject["Name"];
            if (!value.is<std::string>())
            {
                Utility::PrintLine("Expected Name to be a string");
                return false;
            }
            projectName = value.get<std::string>();
        }

        return true;
    }

    bool ParseModules(picojson::object& rootObject)
    {
        return true;
    }
};
