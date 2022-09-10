#pragma once
#include <filesystem>
#include <fstream>
#include <picojson.h>
#include <glob/glob.hpp>
#include "Utility.hpp"

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

    std::vector<std::string> unresolvedDependencies;

    // List of modules this module depends on
    std::vector<ModuleManifest*> moduleDependencies;

    std::vector<std::filesystem::path> sourceFiles;

    std::string uuid = "";

    static std::map<std::string, std::filesystem::path> knownModules;
    static std::map<std::filesystem::path, ModuleManifest*> loadedModules;

    void Load()
    {
        moduleRoot = manifestPath.parent_path();
        name = manifestPath.filename().string();
        name.erase(name.length() - moduleFileSuffix.length());

        knownModules.insert(std::pair<std::string, std::filesystem::path>(Utility::ToLower(name), manifestPath));
        loadedModules.insert(std::pair<std::filesystem::path, ModuleManifest*>(manifestPath, this));

        faroRoot = moduleRoot / ".Faro";
        Utility::EnsureDirectory(faroRoot);
        Utility::HideFolder(faroRoot);

        std::filesystem::path moduleInfo = moduleRoot / ".Faro\\Module";

        std::ifstream uuidFile(moduleInfo / "ModuleId.txt");
        if (uuidFile.is_open())
        {
            uuidFile >> uuid;
            uuidFile.close();
        }

        if (uuid.empty())
        {
            uuid = Utility::GenerateUUID();
            Utility::PrintLineD("Generated new module uuid: " + uuid);
        }

        std::ifstream filesList(moduleInfo / "Source.txt");
        if (filesList.is_open())
        {
            sourceFiles = {};
            for (std::string line; std::getline(filesList, line);)
            {
                sourceFiles.push_back(line);
            }
            filesList.close();
        }

        std::ifstream dependencyList(moduleInfo / "Dependencies.txt");
        if (dependencyList.is_open())
        {
            moduleDependencies = {};
            for (std::string line; std::getline(dependencyList, line);)
            {
                LoadModuleDependency(line);
            }
            dependencyList.close();
        }
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

        std::ofstream uuidFile(moduleInfo / "ModuleId.txt");
        uuidFile << uuid;
        uuidFile.close();

        std::ofstream dependencyList(moduleInfo / "Dependencies.txt");
        for (ModuleManifest* dependency : moduleDependencies)
        {
            dependencyList << dependency->manifestPath.string() << "\n";
        }
        dependencyList.close();
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

        if (!ParseDependencies(rootObject)) return false;

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

    bool ParseDependencies(picojson::object& rootObject)
    {
        unresolvedDependencies = {};
        moduleDependencies = {};

        if (rootObject.find("Dependencies") != rootObject.end())
        {
            picojson::value& value = rootObject["Dependencies"];
            if (!value.is<picojson::array>())
            {
                Utility::PrintLine("Expected Dependencies to be an array");
                return false;
            }

            picojson::array& dependencyArray = value.get<picojson::array>();
            for (picojson::value& dependecyValue : dependencyArray)
            {
                if (!dependecyValue.is<std::string>())
                {
                    Utility::PrintLine("Expected dependency to be a string");
                    return false;
                }

                unresolvedDependencies.push_back(dependecyValue.get<std::string>());
            }
        }

        return true;
    }

    bool ResolveDependencies()
    {
        for (std::string& dependencyName : unresolvedDependencies)
        {
            auto it = knownModules.find(Utility::ToLower(dependencyName));
            if (it != knownModules.end())
            {
                LoadModuleDependency(it->second);
            }
            else
            {
                Utility::PrintLine("Unable to resolve module dependency: " + dependencyName);
                return false;
            }
        }
        return true;
    }

    bool LoadModuleDependency(std::filesystem::path dependencyManifest)
    {
        auto it = loadedModules.find(dependencyManifest);
        if (it != loadedModules.end())
        {
            moduleDependencies.push_back(it->second);
            return true;
        }
        else
        {
            //TODO Attempt to find module in engine or plugins
            Utility::PrintLine("Failed to find module dependency: " + dependencyManifest.string());
            return false;
        }
    }
};

inline std::map<std::string, std::filesystem::path> ModuleManifest::knownModules;
inline std::map<std::filesystem::path, ModuleManifest*> ModuleManifest::loadedModules;
