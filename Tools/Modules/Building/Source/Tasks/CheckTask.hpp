#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override;
};

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};
