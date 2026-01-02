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
    module(module)
{
    buildSteps = { new ModuleCheckStep(this), new ModuleCompileStep(this), new ModuleLinkStep(this) };
    startStep();
}

void ModuleBuild::update()
{
    if (pool.isDone()) startStep(); //TODO Should use TaskGroup?
}

bool ModuleBuild::isDone()
{
    return pool.isDone() && buildStep == nullptr && !shouldContinue;
}

void ModuleBuild::startStep()
{
    if (!shouldContinue) return;

    if (buildStep != nullptr)
    {
        Utility::PrintLineD("Stop build step");
        shouldContinue = buildStep->end();
        delete buildStep;
        buildStep = nullptr;

        if (!shouldContinue)
        {
            Utility::PrintLineD("Build steps wants to stop");
            return;
        }
    }

    if (buildSteps.empty())
    {
        shouldContinue = false;
        Utility::PrintLineD("All steps complete");
        return;
    }
    
    Utility::PrintLineD("Start build step");
    buildStep = buildSteps[0];
    buildSteps.erase(buildSteps.begin());
    buildStep->start();
    
}

ModuleClean::ModuleClean(const BuildSetup& buildSetup, const Toolchain* toolchain, const ModuleManifest* module) :
    buildSetup(buildSetup),
    toolchain(toolchain),
    module(module)
{
}
