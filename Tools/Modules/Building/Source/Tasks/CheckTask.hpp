#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override;
};

class ModuleBinCheckTask : public WorkerTask
{
public:
    ModuleBinCheckTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};

class ModuleDatabaseCheckTask : public WorkerTask
{
public:
    ModuleDatabaseCheckTask(ModuleBuild* info);

    void runTask() override;

    void scheduleScans();

private:
    ModuleBuild* info;
};

class ModuleScanTask : public WorkerTask
{
public:
    ModuleScanTask(ModuleBuild* info, std::filesystem::path file);

    void runTask() override;

private:
    ModuleBuild* info;
    std::filesystem::path file;
};
