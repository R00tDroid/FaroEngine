#pragma once
#include <map>
#include "ModuleTaskInfo.hpp"

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override;

    static std::mutex scannedFilesLock;
    static std::set<std::filesystem::path> scannedFiles;
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
    ModuleDatabaseCheckTask(ModuleCheckStep* step);

    void runTask() override;

    void scheduleScans();

private:
    ModuleCheckStep* step;
};

class ModuleScanTask : public WorkerTask
{
public:
    static void scheduleScan(ModuleCheckStep* step, std::filesystem::path file);

    ModuleScanTask(ModuleCheckStep* step, std::filesystem::path file);

    void runTask() override;

private:
    ModuleCheckStep* step;
    std::filesystem::path file;
};
