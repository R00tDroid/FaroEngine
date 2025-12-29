#include "CheckTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleCheckStep::start()
{
    moduleBuild()->pool.addTask<ModuleCheckTask>(moduleBuild());
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
            Utility::PrintLineD("Checking source " + file.string());

            std::filesystem::path binary = info->module->getObjPath(info->buildSetup, info->toolchain, file);
            if (std::filesystem::exists(binary))
            {
                //TODO Check file dates
            }
            else
            {
                Utility::PrintLineD("Binary missing for " + file.string());
                //TODO File need to be compiled
            }
        }
        else if (Utility::IsIncludeFile(file.string().c_str()))
        {
            //Utility::PrintLineD("Checking include " + file.string());
        }
        else
        {
            //Utility::PrintLineD("Checking unknown " + file.string());
        }

        //TODO Check if source object file exists
        //TODO Check for changes in include tree
    }
}
