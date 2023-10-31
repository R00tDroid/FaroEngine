#include "FileChanges.hpp"
#include <Utility.hpp>
#include <Manifests/ModuleManifest.hpp>

FileTimeDatabase::FileTimeDatabase(ModuleManifest* inModule) : module(inModule)
{
}

void FileTimeDatabase::LoadDatabase()
{
    ClearDatabase();

    std::filesystem::path storageFile = GetStoragePath();
    if (std::filesystem::exists(storageFile))
    {
        std::ifstream stream(storageFile);
        if (stream.is_open())
        {
            while (true)
            {
                //TODO Stop on file end

                unsigned int pathLength = 0;
                stream >> pathLength;

                char pathBuffer[MAX_PATH];
                memset(pathBuffer, 0, MAX_PATH);
                for (int i = 0; i < pathLength; i++)
                {
                    stream >> pathBuffer[i];
                }

                std::filesystem::path path(pathBuffer);

                unsigned long long fileTime;
                stream >> fileTime;
                std::filesystem::file_time_type time = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(fileTime));

                fileDatabase.insert(std::pair<std::filesystem::path, FileChangeInfo>(path, { path, time }));
            }
        }
    }
}

void FileTimeDatabase::SaveDatabase()
{
    std::filesystem::path storageFile = GetStoragePath();

    std::ofstream stream(storageFile);
    if (stream.is_open())
    {
        for (auto& it : fileDatabase) 
        {
            std::string pathString = it.first.string();
            unsigned int pathLength = pathString.length();
            stream << pathLength;

            for (int i = 0; i < pathLength; i++)
            {
                char c = pathString[i];
                stream << c;
            }

            unsigned long long time = it.second.fileTime.time_since_epoch().count();
            stream << time;
        }
    }
}

void FileTimeDatabase::ClearDatabase()
{
    fileDatabase = {};
}

void FileTimeDatabase::CheckForChanges()
{
    for (auto& it : fileDatabase)
    {
        if (std::filesystem::exists(it.first))
        {
            std::filesystem::file_time_type currentFileTime = std::filesystem::last_write_time(it.first);
            it.second.fileChanged = it.second.fileTime != currentFileTime;
        }
        else
        {
            it.second.fileChanged = true;
        }
    }
}

void FileTimeDatabase::SetFileTime(const std::filesystem::path& path)
{
    std::filesystem::file_time_type currentFileTime = std::filesystem::last_write_time(path);

    auto it = fileDatabase.find(path);
    if (it == fileDatabase.end())
    {
        fileDatabase.insert(std::pair<std::filesystem::path, FileChangeInfo>(path, { path, currentFileTime }));
    }
    else
    {
        it->second.fileTime = currentFileTime;
    }
}

void FileTimeDatabase::InvalidateFileTime(const std::filesystem::path& path)
{
    auto it = fileDatabase.find(path);
    if (it == fileDatabase.end())
    {
        fileDatabase.insert(std::pair<std::filesystem::path, FileChangeInfo>(path, { path, {} }));
    }
    else
    {
        it->second.fileTime = {};
    }
}

bool FileTimeDatabase::HasFileChanged(const std::filesystem::path& path)
{
    auto it = fileDatabase.find(path);
    if (it == fileDatabase.end()) return true;
    return it->second.fileChanged;
}

bool FileTimeDatabase::HasAnyFileChanged()
{
    for (auto& it : fileDatabase)
    {
        if (it.second.fileChanged) return true;
    }
    return false;
}

std::filesystem::path FileTimeDatabase::GetStoragePath()
{
    return module->faroDirectory / "ChangeDB.bin";
}
