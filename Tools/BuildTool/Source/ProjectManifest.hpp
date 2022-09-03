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
    std::filesystem::path faroRoot = "";

    std::string projectName = "";
    std::vector<ModuleManifest*> projectModules;

    std::string uuid = "";

    bool Parse(std::filesystem::path path)
    {
        manifestPath = path;
        projectDirectory = manifestPath.parent_path();

        faroRoot = projectDirectory / ".Faro";
        Utility::EnsureDirectory(faroRoot);
        Utility::HideFolder(faroRoot);

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

        //TODO load from file
        if (uuid.empty())
        {
            uuid = Utility::GenerateUUID();
            //TODO save to file
        }

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
        if (rootObject.find("Modules") != rootObject.end())
        {
            picojson::value& value = rootObject["Modules"];
            if (!value.is<picojson::array>())
            {
                Utility::PrintLine("Expected Modules to be an array");
                return false;
            }

            picojson::array& moduleArray = value.get<picojson::array>();
            for (picojson::value& moduleValue : moduleArray)
            {
                if (!moduleValue.is<std::string>())
                {
                    Utility::PrintLine("Expected module path to be a string");
                    return false;
                }

                std::filesystem::path moduleRoot = projectDirectory / moduleValue.get<std::string>();
                moduleRoot.make_preferred();

                bool foundManifest = false;
                for (auto const& entry : std::filesystem::directory_iterator(moduleRoot))
                {
                    if (std::filesystem::is_regular_file(entry))
                    {
                        std::string path = entry.path().string().substr();
                        if (path.length() > ModuleManifest::moduleFileSuffix.length() && path.substr(path.length() - ModuleManifest::moduleFileSuffix.length()) == ModuleManifest::moduleFileSuffix)
                        {
                            foundManifest = true;
                            ModuleManifest* moduleManifest = new ModuleManifest;
                            moduleManifest->manifestPath = entry.path();
                            moduleManifest->project = this;
                            moduleManifest->Load();
                            projectModules.push_back(moduleManifest);
                            break;
                        }
                    }
                }

                if (!foundManifest)
                {
                    Utility::PrintLine("Could not find module manifest in: " + moduleRoot.string());
                    return false;
                }
            }
        }
        return true;
    }
};
