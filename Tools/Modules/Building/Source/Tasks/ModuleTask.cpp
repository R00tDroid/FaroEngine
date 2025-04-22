#include "ModuleTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

ModuleCheckTask::ModuleCheckTask(ModuleBuild* info) : info(info) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(info->module->name()));
    //TODO Check for changes and missing output files
}
