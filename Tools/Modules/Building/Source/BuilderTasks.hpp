#pragma once
#include "ModuleInfo.hpp"
#include "Worker.hpp"

struct BuilderInfo
{
    WorkerPool& pool;
};

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(BuilderInfo& info, const ModuleManifest* module);

    void runTask() override;

private:
    BuilderInfo& info;
    const ModuleManifest* module;
};
