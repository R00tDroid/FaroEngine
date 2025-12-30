#include "FileTree.hpp"

FileTree fileTree;

void FileTree::addFile(std::filesystem::path file, std::vector<std::filesystem::path> branches)
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
