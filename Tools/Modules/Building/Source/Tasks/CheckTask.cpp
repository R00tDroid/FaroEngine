#include "CheckTask.hpp"
#include "Utility.hpp"
#include <fstream>
#include "FileTree.hpp"

std::mutex ModuleCheckStep::scannedFilesLock;
std::set<std::filesystem::path> ModuleCheckStep::scannedFiles;

void ModuleCheckStep::scheduleTreeScan()
{
    treeScannedLock.lock();
    bool alreadyScanned = treeScanned;
    treeScanned = true;
    treeScannedLock.unlock();

    if (alreadyScanned) return;

    for (unsigned int sourceIndex = 0; sourceIndex < moduleBuild()->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = moduleBuild()->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            Utility::PrintLineD("Schedule scan for " + std::string(file.string().c_str()));
            ModuleScanTask::scheduleScan(this, file);
        }
    }
}

void ModuleCheckStep::scheduleBinCheck()
{
    moduleBuild()->pool.addTask<ModuleBinCheckTask>(this);
}

void ModuleCheckStep::scheduleChangeCheck()
{
    moduleBuild()->pool.addTask<ModuleDatabaseCheckTask>(this);
}

ModuleCheckStep::ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent), changes(parent->module->getChangeDB(parent->buildSetup)) {}

void ModuleCheckStep::start()
{
    Utility::PrintLineD("Checking for changes in " + std::string(moduleBuild()->module->name()));
    scheduleBinCheck();
    scheduleChangeCheck();
}

bool ModuleCheckStep::end()
{
    std::set<std::filesystem::path> files = {};
    for (unsigned int sourceIndex = 0; sourceIndex < moduleBuild()->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = moduleBuild()->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            bool anyChanged = false;
            std::vector branches = fileTree.branches(file);
            for (const std::filesystem::path& branch : branches)
            {
                files.insert(branch);

                if (changes.hasChanged(branch))
                {
                    Utility::PrintLineD("Change detected in " + branch.string() + " for " + file.string());
                    anyChanged = true;
                }
            }

            if (anyChanged) {
                moduleBuild()->sourcesToCompileLock.lock();
                moduleBuild()->sourcesToCompile.insert(file);
                moduleBuild()->sourcesToCompileLock.unlock();
            }
        }
    }

    // Delete cache to force the source to be compiled in case the build fails but marks the file tree as up-to-date
    moduleBuild()->sourcesToCompileLock.lock();
    for (const std::filesystem::path& source : moduleBuild()->sourcesToCompile)
    {
        std::filesystem::path binary = moduleBuild()->module->getObjPath(moduleBuild()->buildSetup, moduleBuild()->toolchain, source);
        if (std::filesystem::exists(binary))
        {
            std::filesystem::remove(binary);
        }
    }
    moduleBuild()->sourcesToCompileLock.unlock();

    bool anyChanges = !moduleBuild()->sourcesToCompile.empty();

    if (anyChanges)
    {
        changes.save(files);
    }
    else {
        Utility::PrintLine("No changes detected in " + std::string(moduleBuild()->module->name()));
    }

    return anyChanges;
}

ModuleBinCheckTask::ModuleBinCheckTask(ModuleCheckStep* step) : step(step) {}

void ModuleBinCheckTask::runTask()
{
    Utility::PrintLineD("Check module binaries " + std::string(step->moduleBuild()->module->name()));

    for (unsigned int sourceIndex = 0; sourceIndex < step->moduleBuild()->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = step->moduleBuild()->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            bool needsCompile = false;

            std::filesystem::path binary = step->moduleBuild()->module->getObjPath(step->moduleBuild()->buildSetup, step->moduleBuild()->toolchain, file);
            if (!std::filesystem::exists(binary))
            {
                Utility::PrintLineD("Binary missing for " + file.string());
                needsCompile = true;
            }

            if (needsCompile)
            {
                step->moduleBuild()->sourcesToCompileLock.lock();
                step->moduleBuild()->sourcesToCompile.insert(file);
                step->moduleBuild()->sourcesToCompileLock.unlock();

                step->scheduleTreeScan();
            }
        }
    }
}

ModuleDatabaseCheckTask::ModuleDatabaseCheckTask(ModuleCheckStep* step) : step(step) {}

void ModuleDatabaseCheckTask::runTask()
{
    Utility::PrintLineD("Check module changes " + std::string(step->moduleBuild()->module->name()));

    ChangeDB& changes = step->changes;
    if (changes.load())
    {
        if (changes.anyChanges())
        {
            Utility::PrintLineD("Found changes in database");
        }
        else {
            Utility::PrintLineD("No changes in database found");
            return;
        }
    }
    else
    {
        Utility::PrintLineD("No change database");
    }

    step->scheduleTreeScan();
}

void ModuleScanTask::scheduleScan(ModuleCheckStep* step, std::filesystem::path file)
{
    ModuleCheckStep::scannedFilesLock.lock();
    auto it = ModuleCheckStep::scannedFiles.find(file);
    bool alreadyScanned = it != ModuleCheckStep::scannedFiles.end();
    ModuleCheckStep::scannedFilesLock.unlock();
    
    if (!alreadyScanned) step->moduleBuild()->pool.addTask<ModuleScanTask>(step, file);
}

ModuleScanTask::ModuleScanTask(ModuleCheckStep* step, std::filesystem::path file) : step(step), file(file)
{
    ModuleCheckStep::scannedFilesLock.lock();
    ModuleCheckStep::scannedFiles.insert(file);
    ModuleCheckStep::scannedFilesLock.unlock();
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

    fileTree.addFile(file, std::vector(includes.begin(), includes.end()));
}
