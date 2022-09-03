#pragma once
#include "ProjectManifest.hpp"

// Interface for a task. Tasks define a specific action that will be performed (generate solution, build project, clean project, etc).
class ITask
{
public:
    virtual ~ITask() = default;

    // Get the task priority. This is used to determine the execution order (clean before build for exmaple).
    virtual int GetPriority() const = 0;

    // Perform the task action/
    virtual bool Run(ProjectManifest& project) = 0;
};
