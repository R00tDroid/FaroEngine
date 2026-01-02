#include "FileTree.hpp"

std::vector<std::filesystem::path> FileTree::branches(const std::filesystem::path& path)
{
    std::set<std::filesystem::path> scanned;

    std::set<std::filesystem::path> queue = { path };
    std::set<std::filesystem::path> branches;

    while (!queue.empty())
    {
        std::filesystem::path file = *queue.begin();
        queue.erase(queue.begin());

        if (scanned.find(file) != scanned.end()) continue;

        scanned.insert(file);
        branches.insert(file);

        const auto& it = files.find(file);
        if (it != files.end())
        {
            for (const std::filesystem::path& branch : it->second->branches)
            {
                queue.insert(branch);
            }
        }
    }

    return std::vector(branches.begin(), branches.end());
}

FileTree fileTree;

void FileTree::addFile(const std::filesystem::path& file, std::vector<std::filesystem::path> branches)
{
    filesLock.lock();

    auto it = files.find(file);
    if (it == files.end()) { files.insert(std::pair(file, new FileTreeEntry())); }

    FileTreeEntry* entry = files[file];
    entry->file = file;

    for (const std::filesystem::path& branch : branches)
    {
        entry->branches.insert(branch);

        auto branchIt = files.find(branch);
        if (branchIt == files.end()) { files.insert(std::pair(branch, new FileTreeEntry())); }

        FileTreeEntry* branchEntry = files[branch];
        branchEntry->trunks.insert(file);
    }

    filesLock.unlock();
}
