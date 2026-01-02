#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCleanTask : public WorkerTask
{
public:
    ModuleCleanTask(const ModuleClean* moduleClean);

    void runTask() override;

private:
    const ModuleClean* moduleClean;
};
