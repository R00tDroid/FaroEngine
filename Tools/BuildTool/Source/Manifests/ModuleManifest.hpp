#pragma once
#include <filesystem>
#include <picojson.h>
#include <glob/glob.hpp>
#include <set>
#include "FileChanges.hpp"
#include "ManifestInterface.hpp"
#include "FileTree.hpp"

class ProjectManifest;

enum ModuleType
{
    MT_Library,
    MT_Executable
};

class ModuleManifest : public IManifest
{
public:
    inline static std::string moduleFileSuffix = ".module.json";

    // Name of the module
    std::string name = "";

    // Location within the solution hierarchy
    std::filesystem::path solutionLocation = "";

    // Project this module belongs to
    ProjectManifest* project = nullptr;

    std::vector<std::string> linkingLibraries;

    ModuleType type = MT_Library;

    // List of modules this module depends on
    std::vector<std::filesystem::path> moduleDependencies;

    std::vector<std::filesystem::path> privateIncludes;
    std::vector<std::filesystem::path> publicIncludes;

    std::vector<std::filesystem::path> sourceFiles;

    std::string uuid = "";

    FileTimeDatabase fileDates;

    static ModuleManifest* Parse(std::filesystem::path path);

    static ModuleManifest* LoadFromCache(std::filesystem::path path);

    static ModuleManifest* GetLoadedModule(std::filesystem::path path);

    std::vector<std::filesystem::path> GetPublicIncludeTree();

    std::vector<std::filesystem::path> GetModuleIncludeDirectories();

    static std::string GetModuleName(const std::filesystem::path& path);

    // Get list of direct dependencies
    std::set<ModuleManifest*> GetDependencies();
    // Get a list of all dependencies in this branch of the tree
    std::set<ModuleManifest*> GetDependencyTree();

protected:
    ModuleManifest(const std::filesystem::path& path);

    ~ModuleManifest();

private:
    static std::map<std::filesystem::path, ModuleManifest*> loadedModules;

    bool ParseSourceFiles(picojson::object& rootObject);

    bool ParseDependencies(picojson::object& rootObject);

    bool ParseIncludeDirectories(picojson::object& rootObject, std::string tag, std::vector<std::filesystem::path>& output);

    bool ParseModuleType(picojson::object& rootObject);

    bool ParseSolutionLocation(picojson::object& rootObject);

    bool ParseLinkerLibraries(picojson::object& rootObject);

    void SaveCache();
};

inline std::map<std::filesystem::path, ModuleManifest*> ModuleManifest::loadedModules;
