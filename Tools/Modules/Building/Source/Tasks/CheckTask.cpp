#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleCheckStep::start()
{
    moduleBuild()->pool.addTask<ModuleBinCheckTask>(moduleBuild());
    moduleBuild()->pool.addTask<ModuleDatabaseCheckTask>(moduleBuild());
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

ModuleDatabaseCheckTask::ModuleDatabaseCheckTask(ModuleBuild* info) : info(info) {}

void ModuleDatabaseCheckTask::runTask()
{
    //TODO Check for saved database
    Utility::PrintLineD("Check module changes " + std::string(info->module->name()));

    //TODO If no database, or changes detected, scan tree
    scheduleScans();
}

void ModuleDatabaseCheckTask::scheduleScans()
{
    for (unsigned int sourceIndex = 0; sourceIndex < info->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = info->module->sourceFile(sourceIndex);

        if (Utility::IsSourceFile(file.string().c_str()))
        {
            Utility::PrintLineD("Schedule scan for " + std::string(file.string().c_str()));
            info->pool.addTask<ModuleScanTask>(info, file);
        }
    }
}

ModuleScanTask::ModuleScanTask(ModuleBuild* info, std::filesystem::path file) : info(info), file(file) {}

void ModuleScanTask::runTask()
{
    Utility::PrintLineD("Scanning " + std::string(file.string().c_str()));
}
