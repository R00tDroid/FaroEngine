#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"
#include <fstream>

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
            ModuleScanTask::scheduleScan(step, file);
        }
    }

    //TODO Await tree and check dates
}

void ModuleScanTask::scheduleScan(ModuleCheckStep* step, std::filesystem::path file)
{
    step->scannedFilesLock.lock();
    auto it = step->scannedFiles.find(file);
    bool alreadyScanned = it != step->scannedFiles.end();
    step->scannedFilesLock.unlock();
    
    if (!alreadyScanned) step->moduleBuild()->pool.addTask<ModuleScanTask>(step, file);
}

ModuleScanTask::ModuleScanTask(ModuleCheckStep* step, std::filesystem::path file) : step(step), file(file)
{
    step->scannedFilesLock.lock();
    step->scannedFiles.insert(file);
    step->scannedFilesLock.unlock();
}

void ModuleScanTask::runTask()
{
    Utility::PrintLineD("Scanning " + std::string(file.string().c_str()));

    std::set<std::string> relativeIncludes;
    std::set<std::string> absoluteIncludes;

    std::ifstream fileStream(file);
    std::string line;
    while (std::getline(fileStream, line))
    {
        if (line.find("include") == std::string::npos) continue;

        std::vector<std::string> matches;
        if (Utility::MatchPattern(line, ".*#include.*\"(.*)\".*", matches))
        {
            relativeIncludes.insert(matches[0]);
        }
        else if (Utility::MatchPattern(line, ".*#include.*<(.*)>.*", matches))
        {
            absoluteIncludes.insert(matches[0]);
        }
    }
    fileStream.close();

    std::set<std::filesystem::path> includes;

    std::filesystem::path rootPath = file.parent_path();
    for (const std::string& include : relativeIncludes)
    {
        std::filesystem::path path = rootPath / include;
        if (std::filesystem::exists(path))
        {
            path.make_preferred();
            path = std::filesystem::weakly_canonical(path);

            Utility::PrintLineD("Resolved " + include + " to " + path.string());

            includes.insert(path);
        }
        else
        {
            absoluteIncludes.insert(include); // Try to resolve as an absolute path instead
        }
    }

    std::vector moduleIncludes = step->moduleBuild()->module->moduleIncludes();
    for (const std::string& include : absoluteIncludes)
    {
        bool found = false;
        for (const std::filesystem::path& moduleInclude : moduleIncludes)
        {
            std::filesystem::path path = moduleInclude / include;
            if (std::filesystem::exists(path))
            {
                found = true;
                path.make_preferred();

                Utility::PrintLineD("Resolved " + include + " to " + path.string());

                includes.insert(path);
                break;
            }
        }

        if (!found)
        {
            Utility::PrintLineD("Failed to resolve absolute " + include);
        }
    }

    for (const std::filesystem::path& include : includes)
    {
        scheduleScan(step, include);
    }

    step->fileTree.addFile(file, std::vector(includes.begin(), includes.end()));
}
