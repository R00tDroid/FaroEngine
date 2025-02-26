#include "BuilderTasks.hpp"

#include "Toolchain.hpp"
#include "Utility.hpp"

ModuleCheckTask::ModuleCheckTask(BuilderInfo* info, const ModuleManifest* module) : info(info), module(module) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(module->name()));

    for (unsigned int sourceIndex = 0; sourceIndex < module->sourceFiles(); sourceIndex++)
    {
        info->pool.addTask<CompileTask>(info, module, module->sourceFile(sourceIndex));
    }
}

CompileTask::CompileTask(BuilderInfo* info, const ModuleManifest* module, std::filesystem::path file) : info(info), module(module), file(file) {}

void CompileTask::runTask()
{
    Utility::PrintLineD("Compile " + file.string());
    if (!info->toolchain->compile(info->buildSetup, file.string().c_str()))
    {
        Utility::PrintLine("Failed to compile " + file.string());
    }
}
