#include "BuilderTasks.hpp"
#include "Toolchain.hpp"
#include "Utility.hpp"

ModuleBuild::ModuleBuild(WorkerPool& pool, const BuildSetup& buildSetup, const Toolchain* toolchain, const ModuleManifest* module) :
    pool(pool),
    buildSetup(buildSetup),
    toolchain(toolchain),
    module(module),
    checkStage(pool),
    buildStage(pool),
    linkStage(pool)
{
    checkStage.addTask<ModuleCheckTask>(this, module);
}

void ModuleBuild::update()
{
    if (step == MBS_Check && checkStage.isDone())
    {
        //TODO Skip early if nothing needs to be updated
        step = MBS_Build;

        for (unsigned int sourceIndex = 0; sourceIndex < module->sourceFiles(); sourceIndex++)
        {
            std::filesystem::path file = module->sourceFile(sourceIndex);
            std::string extension = file.extension().string();

            //TODO Check file type with supported source list
            if (extension == ".cpp")
            {
                buildStage.addTask<ModuleCompileTask>(this, module, file);
            }
        }
    }
    else if (step == MBS_Build && buildStage.isDone())
    {
        step = MBS_Link;
        //TODO Schedule link steps
    }
    else if (step == MBS_Link && linkStage.isDone())
    {
        step = MBS_Done;
    }
}

bool ModuleBuild::isDone()
{
    return step == MBS_Done;
}

ModuleCheckTask::ModuleCheckTask(ModuleBuild* info, const ModuleManifest* module) : info(info), module(module) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(module->name()));
    //TODO Check for changes and missing output files
}

ModuleCompileTask::ModuleCompileTask(ModuleBuild* info, const ModuleManifest* module, std::filesystem::path file) : info(info), module(module), file(file) {}

void ModuleCompileTask::runTask()
{
    std::filesystem::path outputPath = module->faroDirectory();
    outputPath /= "Obj";
    outputPath /= info->buildSetup.identifier();
    outputPath /= file.filename().replace_extension(".obj");

    std::string fileString = file.string();
    std::string outString = outputPath.string();
    ToolchainCompileInfo compileInfo = { info->buildSetup, fileString.c_str(), outString.c_str() };

    std::vector<std::string> includeStrings;
    for (const std::filesystem::path& include : module->moduleIncludes())
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
