#include "CleanTask.hpp"
#include "LinkTask.hpp"

ModuleCleanTask::ModuleCleanTask(ModuleClean* moduleClean) : moduleClean(moduleClean)
{
    Utility::PrintLine("Cleaning " + std::string(moduleClean->module->name()));
}

void ModuleCleanTask::runTask()
{
    std::filesystem::path objPath = moduleClean->module->getObjDirectory(moduleClean->buildSetup);

    std::filesystem::path buildPath = moduleClean->module->getBuildDirectory(moduleClean->buildSetup);

    std::filesystem::path binPath = moduleClean->module->getBinPath(moduleClean->buildSetup, moduleClean->toolchain, getModuleLinkType(moduleClean->module->moduleType()));
    
    std::filesystem::path genPath = moduleClean->module->getGeneratedDirectory();

    bool anyError = false;

    try
    {
        if (std::filesystem::exists(objPath))
        {
            if (!std::filesystem::remove_all(objPath))
            {
                Utility::PrintLine("Failed to remove object output");
                anyError = true;
            }
        }

        if (std::filesystem::exists(buildPath))
        {
            if (!std::filesystem::remove_all(buildPath))
            {
                Utility::PrintLine("Failed to remove build output");
                anyError = true;
            }
        }

        if (std::filesystem::exists(binPath))
        {
            if (!std::filesystem::remove_all(binPath))
            {
                Utility::PrintLine("Failed to remove binary output");
                anyError = true;
            }
        }

        if (std::filesystem::exists(genPath))
        {
            if (!std::filesystem::remove_all(genPath))
            {
                Utility::PrintLine("Failed to remove generated output");
                anyError = true;
            }
        }
    }
    catch (std::exception& e)
    {
        Utility::PrintLine("Unknown exception during clean command: " + std::string(e.what()));
        anyError = true;
    }

    if (anyError)
    {
        moduleClean->errorLock.lock();
        moduleClean->error = true;
        moduleClean->errorLock.unlock();
    }
    else
    {
        Utility::PrintLine("Cleaned " + std::string(moduleClean->module->name()));
    }
}
