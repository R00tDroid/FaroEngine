#include "CompileTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleCompileStep::start()
{
    Utility::PrintLineD("Compiling " + std::string(moduleBuild()->module->name()));

    moduleBuild()->sourcesToCompileLock.lock();
    for (const std::filesystem::path& source : moduleBuild()->sourcesToCompile)
    {
        moduleBuild()->pool.addTask<ModuleCompileTask>(this, source);
    }
    moduleBuild()->sourcesToCompileLock.unlock();
}

ModuleCompileTask::ModuleCompileTask(ModuleCompileStep* info, std::filesystem::path file) : info(info), file(file) {}

void ModuleCompileTask::runTask()
{
    std::string fileString = file.string();
    std::string outString = info->info->module->getObjPath(info->info->buildSetup, info->info->toolchain, file).string();
    ToolchainCompileInfo compileInfo = { info->info->buildSetup, fileString.c_str(), outString.c_str() };

    std::vector<std::string> includeStrings;
    for (const std::filesystem::path& include : info->info->module->moduleIncludes())
    {
        includeStrings.push_back(include.string());
    }
    std::vector<const char*> includePaths;
    for (const std::string& include : includeStrings)
    {
        includePaths.push_back(include.c_str());
    }
    compileInfo.includePaths = static_cast<unsigned int>(includePaths.size());
    compileInfo.includePathsPtr = includePaths.data();

    std::vector<std::string> defineStrings = Toolchain::getSetupDefines(info->info->buildSetup);
    std::vector<const char*> defines;
    for (const std::string& define : defineStrings)
    {
        defines.push_back(define.c_str());
    }

    compileInfo.defines = static_cast<unsigned int>(defines.size());
    compileInfo.definesPtr = defines.data();

    std::string log;
    compileInfo.userData = &log;
    compileInfo.onLog = [](void* userData, unsigned int length, const char* string)
    {
        *static_cast<std::string*>(userData) += std::string(string, length);
    };

    bool status = info->info->toolchain->compile(compileInfo);

    std::string message = "> " + fileString;
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
        message += "\nFailed to compile";

        info->resultLock.lock();
        info->success = false;
        info->resultLock.unlock();

        info->moduleBuild()->errorLock.lock();
        info->moduleBuild()->error = true;
        info->moduleBuild()->errorLock.unlock();
    }

    Utility::PrintLine(message);
}
