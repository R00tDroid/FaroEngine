#pragma once
#include "ManifestInterface.hpp"

class ProjectManifest;

enum ModuleType
{
    MT_Library,
    MT_Executable
};

class ModuleManifest : public IManifest
{
public:
    static const char* moduleManifestExtension();

    ModuleManifest(const char* manifestLocation);
    ~ModuleManifest() override;

    // Name of the module
    const char* name();

    // Location within the solution hierarchy
    const char* solutionLocation();

    // Project this module belongs to
    ProjectManifest* project();

    std::vector<std::string> linkingLibraries;

    std::vector<std::string> publicDefines;
    std::vector<std::string> privateDefines;

    ModuleType type = MT_Library;

    // List of modules this module depends on
    std::vector<std::filesystem::path> moduleDependencies;

    std::vector<std::filesystem::path> privateIncludes;
    std::vector<std::filesystem::path> publicIncludes;

    std::vector<std::filesystem::path> sourceFiles;

    std::string platformFilter = "*";

    std::vector<FolderMount> folderMounts;

    std::string uuid = "";

    FileTimeDatabase fileDates;
    FileTree fileTree;

    static ModuleManifest* Parse(std::filesystem::path path, ProjectManifest* project);

    static ModuleManifest* LoadFromCache(std::filesystem::path path, ProjectManifest* project);

    static ModuleManifest* GetLoadedModule(std::filesystem::path path);

    std::vector<std::filesystem::path> GetPublicIncludeTree() const;

    std::vector<std::filesystem::path> GetModuleIncludeDirectories() const;

    std::vector<std::string> GetPublicDefineTree() const;

    std::vector<std::string> GetModuleDefines() const;

    static std::string GetModuleName(const std::filesystem::path& path);

    // Get list of direct dependencies
    std::set<ModuleManifest*> GetDependencies();
    // Get a list of all dependencies in this branch of the tree
    std::set<ModuleManifest*> GetDependencyTree();

    bool IsCompatible(BuildPlatform* platform) const;

private:
    struct Impl;
    Impl* impl = nullptr;

    bool ParseSourceFiles(picojson::object& rootObject);

    bool ParseDependencies(picojson::object& rootObject);

    bool ParseIncludeDirectories(picojson::object& rootObject, std::string tag, std::vector<std::filesystem::path>& output);

    bool ParseDefines(picojson::object& rootObject, std::string tag, std::vector<std::string>& output);

    bool ParseModuleType(picojson::object& rootObject);

    bool ParseSolutionLocation(picojson::object& rootObject);

    bool ParseLinkerLibraries(picojson::object& rootObject);

    bool ParsePlatformFilter(picojson::object& rootObject);

    bool ParseMounts(picojson::object& rootObject);

    void SaveCache();
};
