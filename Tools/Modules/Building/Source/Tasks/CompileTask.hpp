#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCompileStep : public BuildStepInterface
{
public:
    ModuleCompileStep(ModuleBuild* parent) : BuildStepInterface(parent), info(parent) {}
    void start() override;
    bool end() override { return success; }

    ModuleBuild* info;

    std::mutex resultLock;
    bool success = true;
};

class ModuleCompileTask : public WorkerTask
{
public:
    ModuleCompileTask(ModuleCompileStep* info, std::filesystem::path file);

    void runTask() override;

private:
    ModuleCompileStep* info;
    std::filesystem::path file;
};
