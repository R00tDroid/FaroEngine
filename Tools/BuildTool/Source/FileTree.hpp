#pragma once
#include <filesystem>
#include <map>

struct FileTimeInfo
{
    std::filesystem::path file;

    std::filesystem::file_time_type storedTime = {};
    std::filesystem::file_time_type currentTime = {};
    std::filesystem::file_time_type saveTime = {};

    bool hasChanged = false;
    bool hasChildChanged = false;

    std::vector<FileTimeInfo*> includeChildren;
    std::vector<FileTimeInfo*> includeParents;
};

class ModuleManifest;

class ModuleFileDates
{
public:
    ModuleFileDates(ModuleManifest*);

    void ParseFiles();

    bool HasFileChanged(std::filesystem::path file);

    bool HasTreeChanged(std::filesystem::path file);

    void MarkFileInvalid(std::filesystem::path file);
    void MarkFileUpdate(std::filesystem::path file);

    void Save();

    std::vector<FileTimeInfo*> fileInfo;
    std::map<std::filesystem::path, FileTimeInfo*> fileByPath;

private:
    std::string GetFileIdentifier(const std::filesystem::path&);
    std::filesystem::path GetStoragePath();
    FileTimeInfo* FindFile(std::filesystem::path&);

    ModuleManifest* module = nullptr;
};
