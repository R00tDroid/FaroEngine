#include <vector>
#include "Parameters.hpp"
#include "Utility.hpp"
#include "Tasks/ITask.hpp"
#include "Commandline.hpp"

bool RunTasks(std::vector<ITask*>& tasks, TaskRunInfo& runInfo)
{
    std::sort(tasks.begin(), tasks.end(), [](const ITask* A, const ITask* B) { return A->GetPriority() < B->GetPriority(); });

    for (ITask* task : tasks)
    {
        PerformanceTimer taskTimer;
        Utility::PrintLineD("Executing task: " + task->GetTaskName());
        if (!task->Run(runInfo)) return false;
        taskTimer.Stop("Task (" + task->GetTaskName() + ")");
    }
    Utility::PrintLine("Done");

    return true;
}

#if FaroToolTest
#include <gtest/gtest.h>
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();    
}
#else
int main(int argc, char** argv)
{
    PerformanceTimer::StartGlobalTimer();
    PerformanceTimer mainTimer;

    ParameterList parameters(argc, const_cast<const char**>(argv));

    std::vector<ITask*> tasks;

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    TaskRunInfo taskInfo;

    if (!ParseParameters(parameters, tasks, taskInfo))
    {
        return -1;
    }

    if (!tasks.empty())
    {
        if (!RunTasks(tasks, taskInfo))
        {
            return -1;
        }

        mainTimer.Stop("Main");

#ifndef NDEBUG
        PerformanceTimer::PrintTimers();
#endif

        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
#endif
