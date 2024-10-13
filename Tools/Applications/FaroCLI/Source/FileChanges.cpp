#include "FileChanges.hpp"
#include <Utility.hpp>
#include <Manifests/ModuleManifest.hpp>
#include <fstream>

#define MaxPathLength 2048

FileTimeDatabase::FileTimeDatabase(ModuleManifest* inModule) : module(inModule)
{
}

void FileTimeDatabase::SetBuildType(BuildType inBuildType)
{
    buildType = inBuildType;
}

void FileTimeDatabase::LoadDatabase()
{
    assert(buildType != ENUMSIZE);

    ClearDatabase();

    std::filesystem::path storageFile = GetStoragePath();
    if (std::filesystem::exists(storageFile))
    {
        std::ifstream stream(storageFile, std::ios::binary);
        if (stream.is_open())
        {
            while (true)
            {
                if (stream.peek() == EOF) break;

                unsigned int pathLength = 0;
                stream.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));

                assert(pathLength <= MaxPathLength);

                char pathBuffer[MaxPathLength];
                memset(pathBuffer, 0, MaxPathLength);
                for (unsigned int i = 0; i < pathLength; i++)
                {
                    stream >> pathBuffer[i];
                }

                std::filesystem::path path(pathBuffer);

                unsigned long long fileTime;
                stream.read(reinterpret_cast<char*>(&fileTime), sizeof(fileTime));
                std::filesystem::file_time_type time = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(fileTime));

                fileDatabase.insert(std::pair<std::filesystem::path, FileChangeInfo>(path, { path, time }));
            }
        }
    }
}

void FileTimeDatabase::SaveDatabase()
{
    std::filesystem::path storageFile = GetStoragePath();

    std::ofstream stream(storageFile, std::ios::binary);
    if (stream.is_open())
    {
        for (auto& it : fileDatabase) 
        {
            std::string pathString = it.first.string();
            unsigned int pathLength = (unsigned int)pathString.length();
            stream.write(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));

            for (unsigned int i = 0; i < pathLength; i++)
            {
                char c = pathString[i];
                stream << c;
            }

            unsigned long long fileTime = it.second.fileTime.time_since_epoch().count();
            stream.write(reinterpret_cast<char*>(&fileTime), sizeof(fileTime));
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
        if (it.second.fileChanged)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> FileTimeDatabase::GetChangedFiles()
{
    std::vector<std::filesystem::path> files;
    for (auto& it : fileDatabase)
    {
        if (it.second.fileChanged)
        {
            files.push_back(it.first);
        }
    }
    return files;
}

std::filesystem::path FileTimeDatabase::GetStoragePath()
{
    std::string buildTypeName;

    switch (buildType)
    {
        case Debug: { buildTypeName = "Debug"; break; }
        case Development: { buildTypeName = "Development"; break; }
        case Release: { buildTypeName = "Release"; break; }
        case ENUMSIZE:
        default:
        {
            Utility::PrintLineD("Invalid build type");
        }
    }

    return module->faroDirectory / ("ChangeDB_" + buildTypeName + ".bin");
}
