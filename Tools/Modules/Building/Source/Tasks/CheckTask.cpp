#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

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

        auto branchIt = files.find(file);
        if (branchIt == files.end()) { files.insert(std::pair(branch, new FileTreeEntry())); }

        FileTreeEntry* branchEntry = files[branch];
        branchEntry->trunks.insert(file);
    }

    filesLock.unlock();
}

void ModuleCheckStep::start()
{
    moduleBuild()->pool.addTask<ModuleBinCheckTask>(moduleBuild());
    moduleBuild()->pool.addTask<ModuleDatabaseCheckTask>(this);
}

bool ModuleCheckStep::end()
{
    //TODO Prevent duplicates
    return !moduleBuild()->sourcesToCompile.empty();
}

ModuleBinCheckTask::ModuleBinCheckTask(ModuleBuild* info) : info(info) {}

void ModuleBinCheckTask::runTask()
{
    Utility::PrintLineD("Check module binaries " + std::string(info->module->name()));

    for (unsigned int sourceIndex = 0; sourceIndex < info->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = info->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            bool needsCompile = false;

            std::filesystem::path binary = info->module->getObjPath(info->buildSetup, info->toolchain, file);
            if (!std::filesystem::exists(binary))
            {
                Utility::PrintLineD("Binary missing for " + file.string());
                needsCompile = true;
            }

            if (needsCompile) info->sourcesToCompile.push_back(file);
        }
    }
}

ModuleDatabaseCheckTask::ModuleDatabaseCheckTask(ModuleCheckStep* step) : step(step) {}

void ModuleDatabaseCheckTask::runTask()
{
    //TODO Check for saved database
    Utility::PrintLineD("Check module changes " + std::string(step->moduleBuild()->module->name()));

    //TODO If no database, or changes detected, scan tree
    scheduleScans();
}

void ModuleDatabaseCheckTask::scheduleScans()
{
    for (unsigned int sourceIndex = 0; sourceIndex < step->moduleBuild()->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = step->moduleBuild()->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            Utility::PrintLineD("Schedule scan for " + std::string(file.string().c_str()));
            step->moduleBuild()->pool.addTask<ModuleScanTask>(step, file);
        }
    }

    //TODO Await tree and check dates
}

ModuleScanTask::ModuleScanTask(ModuleCheckStep* step, std::filesystem::path file) : step(step), file(file) {}

void ModuleScanTask::runTask()
{
    Utility::PrintLineD("Scanning " + std::string(file.string().c_str()));

    //TODO Scan for includes and resolve to files paths
    step->fileTree.addFile(file, {});
}
