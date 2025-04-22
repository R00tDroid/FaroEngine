#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleLinkTask : public WorkerTask
{
public:
    ModuleLinkTask(ModuleBuild* info);

    void runTask() override;

private:
    ModuleBuild* info;
};
