#pragma once
#include <filesystem>
#include <fstream>
#include <picojson.h>
#include <glob/glob.hpp>

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
    std::filesystem::path faroRoot = "";

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

        faroRoot = moduleRoot / ".Faro";

        //TODO load file list
        //TODO load dependency list
    }

    void Save()
    {
        std::filesystem::path moduleInfo = moduleRoot / ".Faro\\Module";
        Utility::EnsureDirectory(moduleInfo);

        std::ofstream filesList(moduleInfo / "Source.txt");
        for (std::filesystem::path & sourceFile : sourceFiles)
        {
            filesList << sourceFile.string() << "\n";
        }
        filesList.close();
    }

    bool Parse()
    {
        sourceFiles = {};
        moduleDependencies = {};

        std::ifstream fileStream(manifestPath);
        if (!fileStream.is_open())
        {
            Utility::PrintLine("Failed to open module manifest: " + manifestPath.string());
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
            Utility::PrintLine("Expected a JSON object as root of the module manifest");
            return false;
        }

        picojson::object& rootObject = rootValue.get<picojson::object>();

        if (!ParseSourceFiles(rootObject)) return false;

        //TODO parse dependencies

        return true;
    }

    bool ParseSourceFiles(picojson::object& rootObject)
    {
        std::vector<std::filesystem::path> filters;
        if (rootObject.find("SourceFilters") != rootObject.end())
        {
            picojson::value& value = rootObject["SourceFilters"];
            if (!value.is<picojson::array>())
            {
                Utility::PrintLine("Expected SourceFilters to be an array");
                return false;
            }

            picojson::array& filterArray = value.get<picojson::array>();
            for (picojson::value& filterValue : filterArray)
            {
                if (!filterValue.is<std::string>())
                {
                    Utility::PrintLine("Expected filter to be a string");
                    return false;
                }

                std::filesystem::path filter = moduleRoot / filterValue.get<std::string>();
                filter = std::filesystem::weakly_canonical(filter);
                filter.make_preferred();
                filters.push_back(filter);

                while (true)
                {
                    std::string string = filter.string();

                    size_t index = string.find("**");
                    if (index == std::string::npos) break;

                    string[index] = '.';
                    string.erase(index + 1, 1);

                    filter = std::filesystem::weakly_canonical(string);
                    filters.push_back(filter);
                }
            }
        }

        for (std::filesystem::path& filter : filters)
        {
            for (auto& file : glob::rglob(filter.string()))
            {
                sourceFiles.push_back(file);
            }
        }

        sourceFiles.erase(std::unique(sourceFiles.begin(), sourceFiles.end()), sourceFiles.end());
        std::sort(sourceFiles.begin(), sourceFiles.end());

        return true;
    }
};
