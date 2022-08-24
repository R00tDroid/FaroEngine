#pragma once

class ITask
{
public:
    virtual int GetPriority() = 0;

    virtual bool Run(ProjectManifest& project) = 0;
};
