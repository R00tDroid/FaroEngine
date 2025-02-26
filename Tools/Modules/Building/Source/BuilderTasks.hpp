#pragma once
#include <filesystem>

#include "ModuleInfo.hpp"
#include "Worker.hpp"

struct BuilderInfo
{
    WorkerPool& pool;
    const BuildSetup& buildSetup;
    const Toolchain* toolchain;

};

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(BuilderInfo* info, const ModuleManifest* module);

    void runTask() override;

private:
    BuilderInfo* info;
    const ModuleManifest* module;
};

class CompileTask : public WorkerTask
{
public:
    CompileTask(BuilderInfo* info, const ModuleManifest* module, std::filesystem::path file);

    void runTask() override;

private:
    BuilderInfo* info;
    const ModuleManifest* module;
    std::filesystem::path file;
};

