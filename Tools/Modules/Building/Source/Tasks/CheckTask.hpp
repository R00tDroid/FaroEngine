#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override { return true; } //TODO Exit if no updates
};

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};
