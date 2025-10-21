#include "ModuleTaskInfo.hpp"
#include "CompileTask.hpp"
#include "LinkTask.hpp"
#include "CheckTask.hpp"
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
    startCheck();
}

void ModuleBuild::update()
{
    if (step == MBS_Check && checkStage.isDone())
    {
        startBuild();
    }
    else if (step == MBS_Build && buildStage.isDone())
    {
        startLink();
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

void ModuleBuild::startCheck()
{
    checkStage.addTask<ModuleCheckTask>(this);
}

void ModuleBuild::startBuild()
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

void ModuleBuild::startLink()
{
    step = MBS_Link;
    checkStage.addTask<ModuleLinkTask>(this);
    //TODO Stop on error
}
