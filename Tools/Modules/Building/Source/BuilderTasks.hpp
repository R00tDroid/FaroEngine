#pragma once
#include <filesystem>
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

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(ModuleBuild* info, const ModuleManifest* module);

    void runTask() override;

private:
    ModuleBuild* info;
    const ModuleManifest* module;
};

class ModuleCompileTask : public WorkerTask
{
public:
    ModuleCompileTask(ModuleBuild* info, const ModuleManifest* module, std::filesystem::path file);

    void runTask() override;

private:
    ModuleBuild* info;
    const ModuleManifest* module;
    std::filesystem::path file;
};

class ModuleLinkTask : public WorkerTask
{
public:
    ModuleLinkTask(ModuleBuild* info, const ModuleManifest* module);

    void runTask() override;

private:
    ModuleBuild* info;
    const ModuleManifest* module;
};
