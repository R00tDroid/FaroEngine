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
    checkStage.addTask<ModuleCheckTask>(this);
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
            if (Toolchain::needsCompile(file))
            {
                buildStage.addTask<ModuleCompileTask>(this, file);
            }
        }
    }
    else if (step == MBS_Build && buildStage.isDone())
    {
        step = MBS_Link;
        checkStage.addTask<ModuleLinkTask>(this);
        //TODO Stop on error
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

ModuleCheckTask::ModuleCheckTask(ModuleBuild* info) : info(info) {}

void ModuleCheckTask::runTask()
{
    Utility::PrintLineD("Check module " + std::string(info->module->name()));
    //TODO Check for changes and missing output files
}

ModuleCompileTask::ModuleCompileTask(ModuleBuild* info, std::filesystem::path file) : info(info), file(file) {}

static std::filesystem::path getObjPath(const ModuleManifest* module, const BuildSetup& buildSetup, const Toolchain* toolchain, const std::filesystem::path& path)
{
    std::filesystem::path outputPath = module->faroDirectory();
    outputPath /= "Obj";
    outputPath /= buildSetup.identifier();
    outputPath /= path.filename().replace_extension(toolchain->getBinExtension());
    return outputPath;
}

static std::filesystem::path getBinPath(const ModuleManifest* module, const BuildSetup& buildSetup, const Toolchain* toolchain, LinkType type)
{
    std::filesystem::path outputPath = module->faroDirectory();
    outputPath /= "Bin";
    outputPath /= std::string(buildSetup.identifier()) + toolchain->getLinkExtension(type);
    return outputPath;
}

void ModuleCompileTask::runTask()
{
    std::string fileString = file.string();
    std::string outString = getObjPath(info->module, info->buildSetup, info->toolchain, file).string();
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

ModuleLinkTask::ModuleLinkTask(ModuleBuild* info) : info(info) {}

void ModuleLinkTask::runTask()
{
    ToolchainLinkInfo linkInfo = { info->buildSetup };

    switch (info->module->moduleType()) {
        case MT_Library:
        {
            linkInfo.linkType = LT_StaticLibrary;
            break;
        }
        case MT_Executable:
        {
            linkInfo.linkType = LT_Application;
            break;
        }
        default:
        {
            Utility::PrintLine("Unknown module type");
        }
    }

    std::vector<std::string> objFiles;
    for (unsigned int sourceIndex = 0; sourceIndex < info->module->sourceFiles(); sourceIndex++)
    {
        std::filesystem::path file = info->module->sourceFile(sourceIndex);
        if (Toolchain::needsCompile(file))
        {
            objFiles.push_back(getObjPath(info->module, info->buildSetup, info->toolchain, file).string());
        }
    }

    std::vector<const char*> objFilesPaths;
    for (const std::string& path : objFiles)
    {
        objFilesPaths.push_back(path.c_str());
    }
    linkInfo.objFilesPtr = objFilesPaths.data();
    linkInfo.objFiles = static_cast<unsigned int>(objFilesPaths.size());

    std::string outputPath = getBinPath(info->module, info->buildSetup, info->toolchain, linkInfo.linkType).string();
    linkInfo.output = outputPath.c_str();

    std::string log;
    linkInfo.userData = &log;
    linkInfo.onLog = [](void* userData, unsigned int length, const char* string)
    {
        *static_cast<std::string*>(userData) += std::string(string, length);
    };

    bool status = info->toolchain->link(linkInfo);

    std::string message = "Linking " + std::string(info->module->name());
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
    }

    Utility::PrintLine(message);
}
