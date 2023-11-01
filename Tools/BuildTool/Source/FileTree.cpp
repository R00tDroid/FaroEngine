#include "FileTree.hpp"
#include "Manifests/ProjectManifest.hpp"
#include "Manifests/ModuleManifest.hpp"
#include <fstream>
#include <ctime>
#include <chrono>
#include <Utility.hpp>

void FileTreeGenerator::ParseTree(std::vector<ModuleManifest*> modules)
{
    /*std::map<std::filesystem::path, FileTimeInfo*> fileByPath;
    for (ModuleManifest* module : modules)
    {
        for (auto& it : module->fileDates.fileByPath)
        {
            fileByPath.insert(it);
        }
    }

    for (ModuleManifest* module : modules)
    {
        for (std::filesystem::path& file : module->sourceFiles)
        {
            ResolveFiletree(module, file, fileByPath);
        }
    }

    for (ModuleManifest* module : modules)
    {
        for (std::filesystem::path& file : module->sourceFiles)
        {
            FileTimeInfo* fileInfo = module->fileDates.FindFile(file);
            if (fileInfo == nullptr) continue;

            UpdateTreeStatus(fileInfo);
        }
    }*/
}

/*void FileTreeGenerator::UpdateTreeStatus(FileTimeInfo* file)
{
    if (!file->hasChildChanged) file->hasChildChanged = HasChildChanged(file);
}

bool FileTreeGenerator::HasChildChanged(FileTimeInfo* file)
{
    for (FileTimeInfo* child : file->includeChildren)
    {
        UpdateTreeStatus(child);
    }

    for (FileTimeInfo* child : file->includeChildren)
    {
        if (child->hasChanged || child->hasChildChanged) return true;
    }

    return false;
}

void FileTreeGenerator::ResolveFiletree(ModuleManifest* module, std::filesystem::path& file, std::map<std::filesystem::path, FileTimeInfo*>& fileByPath)
{
    FileTimeInfo* fileInfo = module->fileDates.FindFile(file);
    if (fileInfo == nullptr) return;

    std::ifstream stream(file);
    if (!stream.is_open()) return;

    std::string line;
    while (std::getline(stream, line)) //TODO Ignore commented lines
    {
        if (line.find("#include") == std::string::npos) continue;

        std::vector<std::string> matches;
        if (Utility::MatchPattern(line, "(.*)(#include \")(.*)(\")(.*)", matches) || Utility::MatchPattern(line, "(.*)(#include <)(.*)(>)", matches))
        {
            std::string include = matches[2];

            std::vector<std::filesystem::path> directories = module->GetPublicIncludeTree();
            directories.push_back(file.parent_path());

            FileTimeInfo* includeInfo = FindFile(directories, include, fileByPath);
            if (includeInfo != nullptr) 
            {
                fileInfo->includeChildren.push_back(includeInfo);
                includeInfo->includeParents.push_back(fileInfo);
            }
        }
    }
}

FileTimeInfo* FileTreeGenerator::FindFile(std::vector<std::filesystem::path>& directories, std::string name, std::map<std::filesystem::path, FileTimeInfo*>& fileByPath)
{
    for (std::filesystem::path& directory : directories)
    {
        std::filesystem::path path = directory / name;
        path = std::filesystem::weakly_canonical(path);

        auto it = fileByPath.find(path);
        if (it != fileByPath.end())
        {
            return it->second;
        }
    }

    return nullptr;
}*/
