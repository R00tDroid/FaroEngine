#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleCheckStep::start()
{
    moduleBuild()->pool.addTask<ModuleCheckTask>(moduleBuild());
}

bool ModuleCheckStep::end()
{
    return !moduleBuild()->sourcesToCompile.empty();
}

ModuleCheckTask::ModuleCheckTask(ModuleBuild* info) : info(info) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(info->module->name()));
    //TODO Check for changes and missing output files

    std::vector<std::filesystem::path> includes = info->module->moduleIncludes();

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
            else
            {
                //TODO Check for changes in include tree
                needsCompile = true;
            }

            if (needsCompile) info->sourcesToCompile.push_back(file);
        }
    }
}
