#pragma once
#include "ChangeDatabase.hpp"
#include "ModuleTaskInfo.hpp"

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent);
    void start() override;
    bool end() override;

    static std::mutex scannedFilesLock;
    static std::set<std::filesystem::path> scannedFiles;

    ChangeDB changes;

    void scheduleTreeScan();
    void scheduleBinCheck();
    void scheduleChangeCheck();

private:
    std::mutex treeScannedLock;
    bool treeScanned = false;
};

class ModuleBinCheckTask : public WorkerTask
{
public:
    ModuleBinCheckTask(ModuleCheckStep* step);

    void runTask() override;

private:
    ModuleCheckStep* step;
};

class ModuleDatabaseCheckTask : public WorkerTask
{
public:
    ModuleDatabaseCheckTask(ModuleCheckStep* step);

    void runTask() override;

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

// This function return all source files for a module. This includes the usual source files, but also any generated sources.
extern std::vector<std::filesystem::path> getModuleSourceFiles(const ModuleManifest* module);
