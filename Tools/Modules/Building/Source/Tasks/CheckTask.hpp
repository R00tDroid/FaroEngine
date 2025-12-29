#pragma once
#include <map>
#include "ModuleTaskInfo.hpp"

struct FileTreeEntry
{
    std::filesystem::path file;

    std::set<std::filesystem::path> branches;
    std::set<std::filesystem::path> trunks;
};

class FileTree
{
public:
    void addFile(std::filesystem::path file, std::vector<std::filesystem::path> branches);

private:
    std::mutex filesLock;
    std::map<std::filesystem::path, FileTreeEntry*> files;
};

class ModuleCheckStep : public BuildStepInterface
{
public:
    ModuleCheckStep(ModuleBuild* parent) : BuildStepInterface(parent) {}
    void start() override;
    bool end() override;

    std::mutex scannedFilesLock;
    std::vector<std::filesystem::path> scannedFiles;

    FileTree fileTree;
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
    ModuleScanTask(ModuleCheckStep* step, std::filesystem::path file);

    void runTask() override;

private:
    ModuleCheckStep* step;
    std::filesystem::path file;
};
