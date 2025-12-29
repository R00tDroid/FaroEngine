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
    if (pool.isDone()) startStep();
}

bool ModuleBuild::isDone()
{
    return pool.isDone() && buildSteps.empty() && buildStep == nullptr;
}

void ModuleBuild::startStep()
{
    if (buildStep != nullptr)
    {
        Utility::PrintLine("Stop build step");
        buildStep->end();
        delete buildStep;
        buildStep = nullptr;
    }

    if (!buildSteps.empty())
    {
        Utility::PrintLine("Start build step");
        buildStep = buildSteps[0];
        buildSteps.erase(buildSteps.begin());
        buildStep->start();
    }
}
