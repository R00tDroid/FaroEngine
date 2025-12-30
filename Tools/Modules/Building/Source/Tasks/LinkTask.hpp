#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleLinkStep : public BuildStepInterface
{
public:
    ModuleLinkStep(ModuleBuild* parent) : BuildStepInterface(parent), info(parent) {}
    void start() override;
    bool end() override { return success; }

    ModuleBuild* info;

    bool success = true;
};

class ModuleLinkTask : public WorkerTask
{
public:
    ModuleLinkTask(ModuleLinkStep* info);

    void runTask() override;

private:
    ModuleLinkStep* info;
};
