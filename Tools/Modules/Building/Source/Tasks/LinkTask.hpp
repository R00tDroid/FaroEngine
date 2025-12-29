#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleLinkStep : public BuildStepInterface
{
public:
    ModuleLinkStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override { return true; }
};

class ModuleLinkTask : public WorkerTask
{
public:
    ModuleLinkTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};
