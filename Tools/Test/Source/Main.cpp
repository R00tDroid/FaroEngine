#include <vector>
#include "Parameters.hpp"
#include "Utility.hpp"
#include "Tasks/ITask.hpp"

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


#include <gtest/gtest.h>
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();    
}
