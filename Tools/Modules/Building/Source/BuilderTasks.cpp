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
    std::string fileString = file.string();
    ToolchainCompileInfo compileInfo = { info->buildSetup, fileString.c_str() };

    Utility::PrintLineD("Compile " + fileString);
    if (!info->toolchain->compile(compileInfo))
    {
        Utility::PrintLine("Failed to compile " + fileString);
    }
}
