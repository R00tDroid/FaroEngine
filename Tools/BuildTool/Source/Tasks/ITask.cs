using System;
using System.Diagnostics;

public abstract class ITask
{
    public abstract int GetPriority();

    public abstract bool Run(ProjectManifest project);
}