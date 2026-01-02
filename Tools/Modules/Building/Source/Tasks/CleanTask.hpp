#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCleanTask : public WorkerTask
{
public:
    ModuleCleanTask(ModuleClean* moduleClean);

    void runTask() override;

private:
    ModuleClean* moduleClean;
};
