#pragma once
#include <filesystem>
#include <map>
#include <vector>

#include "Toolchains/IToolchain.hpp"

struct FileChangeInfo
{
    std::filesystem::path filePath;

    std::filesystem::file_time_type fileTime = {};
    bool fileChanged = false;
};

class ModuleManifest;

class FileTimeDatabase
{
public:
    FileTimeDatabase(ModuleManifest*);

    void SetBuildType(BuildType buildType);

    void LoadDatabase();
    void SaveDatabase();
    void ClearDatabase();

    void CheckForChanges();

    void SetFileTime(const std::filesystem::path&);
    void InvalidateFileTime(const std::filesystem::path&);

    bool HasFileChanged(const std::filesystem::path&);
    bool HasAnyFileChanged();
    std::vector<std::filesystem::path> GetChangedFiles();

private:
    std::filesystem::path GetStoragePath();

    ModuleManifest* module = nullptr;

    std::map<std::filesystem::path, FileChangeInfo> fileDatabase;

    BuildType buildType = ENUMSIZE;
};
