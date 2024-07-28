#include "BuildConfig.hpp"
#include "Utility.hpp"
#include "Tasks/TaskBuild.hpp"
#include "Tasks/TaskGenerate.hpp"

TaskRunInfo parseConfig(BuilderConfiguration* config)
{
    TaskRunInfo runInfo;

    runInfo.projectManifestPath = config->projectManifestPath;
    runInfo.buildPlatform = config->build.buildPlatform;
    runInfo.buildType = config->build.buildType;

    for (unsigned int i = 0; i < config->modulesCount; i++)
    {
        runInfo.moduleList.push_back(config->modules[i]);
    }

    return runInfo; //TODO Check if project manifest it loaded correctly
}

bool performBuildStages(BuilderConfiguration* configuration)
{
    if (configuration == nullptr) return false;

    TaskRunInfo runInfo = parseConfig(configuration);

    std::vector<ITask*> tasks;

    if (configuration->runGeneration)
    {
        tasks.push_back(new TaskGenerate());
    }

    if (configuration->runBuild)
    {
        if (runInfo.projectManifestPath.empty())
        {
            Utility::PrintLine("'-build' requires a project to be specified"); //TODO Update errors to reflect library
            return false;
        }

        if (runInfo.buildPlatform.empty())
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return false;
        }

        if (runInfo.buildType == BuildType::ENUMSIZE)
        {
            Utility::PrintLine("'-build' requires a build configuration to be specified");
            return false;
        }

        tasks.push_back(new TaskBuild());
    }

    if (tasks.empty()) return true;

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
