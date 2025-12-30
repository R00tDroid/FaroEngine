#pragma once
#include <filesystem>
#include <map>
#include <mutex>
#include <set>

struct FileTreeEntry
{
    std::filesystem::path file;

    std::set<std::filesystem::path> branches;
    std::set<std::filesystem::path> trunks;
};

class FileTree
{
public:
    void addFile(std::filesystem::path file, std::vector<std::filesystem::path> branches);

private:
    std::mutex filesLock;
    std::map<std::filesystem::path, FileTreeEntry*> files;
};

extern FileTree fileTree;
