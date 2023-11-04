#pragma once
#include <filesystem>
#include <map>
#include <set>

class ModuleManifest;

class FileTree
{
public:
    FileTree(ModuleManifest*);
    void Parse();

private:
    static bool FindIncludes(const std::filesystem::path& file, std::set<std::string>& relativeIncludes, std::set<std::string>& absoluteIncludes);

    ModuleManifest* module = nullptr;

    std::map<std::filesystem::path, std::set<std::filesystem::path>> includedFiles;
};
