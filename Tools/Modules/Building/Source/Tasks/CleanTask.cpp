#include "CleanTask.hpp"

ModuleCleanTask::ModuleCleanTask(const ModuleClean* moduleClean) : moduleClean(moduleClean)
{
    Utility::PrintLine("Cleaning " + std::string(moduleClean->module->name()));
}

void ModuleCleanTask::runTask()
{
    //TODO Clean module
}
