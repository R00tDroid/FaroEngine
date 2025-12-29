#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCompileStep : public BuildStepInterface
{
public:
    ModuleCompileStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override { return true; } //TODO Exit on error
};

class ModuleCompileTask : public WorkerTask
{
public:
    ModuleCompileTask(ModuleBuild* info, std::filesystem::path file);

    void runTask() override;

private:
    ModuleBuild* info;
    std::filesystem::path file;
};
