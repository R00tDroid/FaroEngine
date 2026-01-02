#include "LinkTask.hpp"
#include <assert.h>
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleLinkStep::start()
{
    Utility::PrintLineD("Linking " + std::string(moduleBuild()->module->name()));
    moduleBuild()->pool.addTask<ModuleLinkTask>(this);
}

ModuleLinkTask::ModuleLinkTask(ModuleLinkStep* info) : info(info) {}

LinkType getModuleLinkType(ModuleType type)
{
    switch (type) {
    case MT_Library:
        return LT_StaticLibrary;
    case MT_Executable:
        return LT_Application;
    }

    assert(false);
    return LT_StaticLibrary;
}

void ModuleLinkTask::runTask()
{
    ToolchainLinkInfo linkInfo = { info->info->buildSetup, getModuleLinkType(info->info->module->moduleType()) };

    std::vector<std::string> objFiles;
    for (unsigned int sourceIndex = 0; sourceIndex < info->info->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = info->info->module->sourceFile(sourceIndex);
        if (Utility::IsSourceFile(file.string().c_str()))
        {
            objFiles.push_back(info->info->module->getObjPath(info->info->buildSetup, info->info->toolchain, file).string());
        }
    }
    std::vector<const char*> objFilesPaths;
    for (const std::string& path : objFiles)
    {
        objFilesPaths.push_back(path.c_str());
    }
    linkInfo.objFilesPtr = objFilesPaths.data();
    linkInfo.objFiles = static_cast<unsigned int>(objFilesPaths.size());

    std::vector<std::string> moduleLibs;
    std::vector<const char*> linkLibs;
    std::vector<ModuleManifest*> dependencies = info->info->module->moduleDependencies();
    for (ModuleManifest* dependency : dependencies)
    {
        moduleLibs.push_back(dependency->getBinPath(info->info->buildSetup, info->info->toolchain, getModuleLinkType(dependency->moduleType())).string());

        for (unsigned int i = 0; i < dependency->linkerLibraries(); i++)
        {
            linkLibs.push_back(dependency->linkerLibrary(i));
        }
    }

    std::vector<const char*> moduleLibPaths;
    for (const std::string& path : moduleLibs)
    {
        moduleLibPaths.push_back(path.c_str());
    }
    linkInfo.moduleLibsPtr = moduleLibPaths.data();
    linkInfo.moduleLibs = static_cast<unsigned int>(moduleLibPaths.size());

    linkInfo.linkLibsPtr = linkLibs.data();
    linkInfo.linkLibs = static_cast<unsigned int>(linkLibs.size());

    //TODO Register lib directories

    std::string outputPath = info->info->module->getBinPath(info->info->buildSetup, info->info->toolchain, linkInfo.linkType).string();
    linkInfo.output = outputPath.c_str();

    std::string log;
    linkInfo.userData = &log;
    linkInfo.onLog = [](void* userData, unsigned int length, const char* string)
    {
        *static_cast<std::string*>(userData) += std::string(string, length);
    };

    bool status = info->info->toolchain->link(linkInfo);

    std::string message = "Linking " + std::string(info->info->module->name());
    if (!log.empty())
    {
        auto it = log.end() - 1;
        if (*it == '\n')
        {
            log.erase(it);
        }
        message += "\n" + log;
    }

    if (!status)
    {
        message += "\nFailed to link";

        info->success = false;

        info->moduleBuild()->errorLock.lock();
        info->moduleBuild()->error = true;
        info->moduleBuild()->errorLock.unlock();
    }

    Utility::PrintLine(message);
}
