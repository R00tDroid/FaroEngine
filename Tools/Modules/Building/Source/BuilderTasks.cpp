#include "BuilderTasks.hpp"

#include "Toolchain.hpp"
#include "Utility.hpp"

ModuleCheckTask::ModuleCheckTask(BuilderInfo* info, const ModuleManifest* module) : info(info), module(module) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(module->name()));

    for (unsigned int sourceIndex = 0; sourceIndex < module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = module->sourceFile(sourceIndex);
        std::string extension = file.extension().string();

        //TODO Check file type with supported source list
        if (extension == ".cpp")
        {
            info->pool.addTask<CompileTask>(info, module, file);
        }
    }
}

CompileTask::CompileTask(BuilderInfo* info, const ModuleManifest* module, std::filesystem::path file) : info(info), module(module), file(file) {}

void CompileTask::runTask()
{
    std::filesystem::path outputPath = module->faroDirectory();
    outputPath /= "Obj";
    outputPath /= info->buildSetup.identifier();
    outputPath /= file.filename().replace_extension(".obj");

    std::string fileString = file.string();
    std::string outString = outputPath.string();
    ToolchainCompileInfo compileInfo = { info->buildSetup, fileString.c_str(), outString.c_str() };

    std::vector<const char*> includePaths; //TODO Get public includes from dependencies
    for (int includeType = 0; includeType < AD_ENUMSIZE; includeType++)
    {
        AccessDomain type = static_cast<AccessDomain>(includeType);
        for (unsigned int includeIndex = 0; includeIndex < module->includePaths(type); includeIndex++)
        {
            const char* include = module->includePath(type, includeIndex);
            includePaths.push_back(include);
        }
    }
    compileInfo.includePaths = static_cast<unsigned int>(includePaths.size());
    compileInfo.includePathsPtr = includePaths.data();

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
