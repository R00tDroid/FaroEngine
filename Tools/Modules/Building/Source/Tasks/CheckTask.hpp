#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCheckTask : public WorkerTask
{
public:
    ModuleCheckTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};
