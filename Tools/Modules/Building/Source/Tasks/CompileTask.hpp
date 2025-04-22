#pragma once
#include "ModuleTaskInfo.hpp"

class ModuleCompileTask : public WorkerTask
{
public:
    ModuleCompileTask(ModuleBuild* info, std::filesystem::path file);

    void runTask() override;

private:
    ModuleBuild* info;
    std::filesystem::path file;
};
