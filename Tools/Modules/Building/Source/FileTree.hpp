#pragma once
#include <filesystem>
#include <map>
#include <mutex>
#include <set>
#include <vector>

struct FileTreeEntry
{
    std::filesystem::path file;

    std::set<std::filesystem::path> branches;
    std::set<std::filesystem::path> trunks;
};

class FileTree
{
public:
    void addFile(const std::filesystem::path& file, std::vector<std::filesystem::path> branches);

    std::vector<std::filesystem::path> branches(const std::filesystem::path& file);

private:
    std::mutex filesLock;
    std::map<std::filesystem::path, FileTreeEntry*> files;
};

extern FileTree fileTree;
