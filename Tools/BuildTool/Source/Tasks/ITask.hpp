#pragma once
#include <Manifests/ProjectManifest.hpp>
#include <Toolchains/IToolchain.hpp>

struct TaskRunInfo
{
    std::filesystem::path projectManifestPath = "";
    ProjectManifest* projectManifest = nullptr;

    std::string buildPlatform = "";
    std::string buildArchitecture = "";
    BuildType buildType = BuildType::ENUMSIZE;

    std::vector<std::string> moduleList = {};

    ~TaskRunInfo()
    {
        if (projectManifest != nullptr)
        {
            delete projectManifest;
            projectManifest = nullptr;
        }
    }
};

// Interface for a task. Tasks define a specific action that will be performed (generate solution, build project, clean project, etc).
class ITask
{
public:
    virtual ~ITask() = default;

    // Get the task priority. This is used to determine the execution order (clean before build for exmaple).
    virtual int GetPriority() const = 0;

    // Perform the task action/
    virtual bool Run(TaskRunInfo& runInfo) = 0;

    virtual std::string GetTaskName() = 0;
};
