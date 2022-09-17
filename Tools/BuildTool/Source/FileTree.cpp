#include "FileTree.hpp"
#include "ModuleManifest.hpp"
#include <fstream>
#include <ctime>
#include <chrono>

ModuleFileDates::ModuleFileDates(ModuleManifest* inModule) : module(inModule)
{
}

void ModuleFileDates::ParseFiles()
{
    fileInfo = {};
    fileByPath = {};

    std::filesystem::path storageDir = GetStoragePath();

    for (const std::filesystem::path& file : module->sourceFiles)
    {
        FileTimeInfo* info = new FileTimeInfo();
        fileInfo.push_back(info);
        fileByPath.insert(std::pair<std::filesystem::path, FileTimeInfo*>(file, info));

        info->file = file;
        info->currentTime = std::filesystem::last_write_time(file);

        std::string id = GetFileIdentifier(file);
        std::filesystem::path  path = storageDir / (id + ".bin");

        std::ifstream stream(path);
        if (stream.is_open()) 
        {
            unsigned long long fileTime;
            stream >> fileTime;
            info->storedTime = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(fileTime));
        }
    }
}

bool ModuleFileDates::HasFileChanged(std::filesystem::path file)
{
    FileTimeInfo* info = FindFile(file);
    if (info != nullptr)
    {
        return info->storedTime != info->currentTime;
    }

    return true;
}

void ModuleFileDates::MarkFileInvalid(std::filesystem::path file)
{
    FileTimeInfo* info = FindFile(file);
    if (info != nullptr)
    {
        info->saveTime = {};
    }
}

void ModuleFileDates::MarkFileUpdate(std::filesystem::path file)
{
    FileTimeInfo* info = FindFile(file);
    if (info != nullptr)
    {
        info->saveTime = info->currentTime;
    }
}

void ModuleFileDates::Save()
{
    std::filesystem::path storageDir = GetStoragePath();
    Utility::EnsureDirectory(storageDir);

    for (const FileTimeInfo* info : fileInfo)
    {
        std::string id = GetFileIdentifier(info->file);
        std::filesystem::path  path = storageDir / (id + ".bin");

        std::ofstream stream(path);
        unsigned long long time = info->saveTime.time_since_epoch().count();
        stream << time;
    }
}

std::string ModuleFileDates::GetFileIdentifier(const std::filesystem::path& file)
{
    std::string path = std::filesystem::proximate(file, module->moduleRoot).string();
    std::replace(path.begin(), path.end(), '\\', '_');
    std::replace(path.begin(), path.end(), '/', '_');
    std::replace(path.begin(), path.end(), '.', '_');
    path = Utility::ToLower(path);
    return path;
}

std::filesystem::path ModuleFileDates::GetStoragePath()
{
    return module->faroRoot / "ChangeDB";
}

FileTimeInfo* ModuleFileDates::FindFile(std::filesystem::path& path)
{
    auto it = fileByPath.find(path);
    if (it == fileByPath.end())
    {
        return nullptr;
    }
    else
    {
        return it->second;
    }
}
