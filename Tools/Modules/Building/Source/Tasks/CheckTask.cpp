#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"
#include <fstream>
#include "FileTree.hpp"

std::mutex ModuleCheckStep::scannedFilesLock;
std::set<std::filesystem::path> ModuleCheckStep::scannedFiles;

void ModuleCheckStep::start()
{
    moduleBuild()->pool.addTask<ModuleBinCheckTask>(moduleBuild());
    moduleBuild()->pool.addTask<ModuleDatabaseCheckTask>(this);
}

bool ModuleCheckStep::end()
{
    //TODO Check tree scan results
    //TODO Prevent duplicates

    // Delete cache to force the source to be compiled in case the build fails but marks the file tree as up-to-date
    for (const std::filesystem::path& source : moduleBuild()->sourcesToCompile)
    {
        std::filesystem::path binary = moduleBuild()->module->getObjPath(moduleBuild()->buildSetup, moduleBuild()->toolchain, source);
        if (std::filesystem::exists(binary))
        {
            std::filesystem::remove(binary);
        }
    }

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
