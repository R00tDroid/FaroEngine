#pragma once
#include "ModuleInfo.hpp"
#include "Worker.hpp"

enum ModuleBuildSteps
{
    MBS_Check,
    MBS_Build,
    MBS_Link,
    MBS_Done
};

class ModuleBuild
{
public:
    ModuleBuild(WorkerPool& pool, const BuildSetup& buildSetup, const Toolchain* toolchain, const ModuleManifest* module);

    WorkerPool& pool;
    const BuildSetup& buildSetup;
    const Toolchain* toolchain;
    const ModuleManifest* module;

    void update();
    bool isDone();

    ModuleBuildSteps step = MBS_Check;

private:
    TaskGroup checkStage;
    TaskGroup buildStage;
    TaskGroup linkStage;
};
