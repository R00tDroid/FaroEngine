#include "CompileTask.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

void ModuleCompileStep::start()
{
    moduleBuild()->sourcesToCompileLock.lock();
    for (const std::filesystem::path& source : moduleBuild()->sourcesToCompile)
    {
        moduleBuild()->pool.addTask<ModuleCompileTask>(moduleBuild(), source);
    }
    moduleBuild()->sourcesToCompileLock.unlock();
}

ModuleCompileTask::ModuleCompileTask(ModuleBuild* info, std::filesystem::path file) : info(info), file(file) {}

void ModuleCompileTask::runTask()
{
    std::string fileString = file.string();
    std::string outString = info->module->getObjPath(info->buildSetup, info->toolchain, file).string();
    ToolchainCompileInfo compileInfo = { info->buildSetup, fileString.c_str(), outString.c_str() };

    std::vector<std::string> includeStrings;
    for (const std::filesystem::path& include : info->module->moduleIncludes())
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

    std::vector<std::string> defineStrings;
    for (unsigned int i = 0; i < Toolchain::defines(info->buildSetup); i++)
    {
        defineStrings.push_back(Toolchain::define(info->buildSetup, i));
    }

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);
            bool isCurrent = target == info->buildSetup.target;

            for (unsigned int defineIndex = 0; defineIndex < target->defines(); defineIndex++)
            {
                std::string define = target->define(defineIndex);
                defineStrings.push_back(define + "=" + (isCurrent ? "1" : "0"));
            }
        }
    }

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

    bool status = info->toolchain->compile(compileInfo);

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
    }

    Utility::PrintLine(message);
}
