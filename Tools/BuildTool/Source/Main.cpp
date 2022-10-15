#include <vector>
#include "Parameters.hpp"
#include "ProjectManifest.hpp"
#include "Utility.hpp"
#include "Version.generated.hpp"
#include "Tasks/ITask.hpp"
#include "Tasks/TaskGenerate.hpp"
#include "Tasks/TaskBuild.hpp"

void PrintHelp()
{
    Utility::Print("Faro Engine build tool " + std::string(EngineVersion) + "\n\n");
    Utility::Print("Tasks:\n");
    Utility::Print("-build    Compile the specified source\n");
    Utility::Print("-clean        Clean intermediates\n");
    Utility::Print("-generate    Generate Visual Studio project\n");
    Utility::Print("-deploy        Assemble build for specified project\n\n");

    Utility::Print("Compilation: -compile\n");
    Utility::Print("-project <path>                                        The project to compile\n");
    Utility::Print("-module <module1, module2>                            Modules to be compile\n");
    Utility::Print("-debug / -development / -release                        The configuration to compile with (debug / deployment / release)\n");
    Utility::Print("-platform <platform type> <architecture>             Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("Cleaning: -clean\n");
    Utility::Print("-project <path>                                        The project to clean\n");
    Utility::Print("-module <module1, module2>                            Modules to be cleaned\n");
    Utility::Print("-debug / -development / -release                    The configuration to build or clean (debug / development / release)\n");
    Utility::Print("-platform <platform type> <architecture> [flavor]    Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("solution generation: -generate\n");
    Utility::Print("-project <path>    The project to generate solution files for\n");
}

struct CommandInfo
{
    std::filesystem::path projectPath = "";
    ProjectManifest projectManifest;

    std::string buildPlatform = "";
    std::string buildArchitecture = "";
    BuildType buildType = BuildType::ENUMSIZE;
};

bool ParseParameters(ParameterList& parameters, std::vector<ITask*>& tasks, CommandInfo& commandInfo)
{
    if (parameters.HasArguments("project"))
    {
        try
        {
            commandInfo.projectPath = std::filesystem::weakly_canonical(parameters.GetArguments("project")[0]);
            commandInfo.projectPath.make_preferred();
        }
        catch (std::exception const& e)
        {
            Utility::PrintLine("Failed to locate project: " + parameters.GetArguments("project")[0] + " (" + e.what() + ")");
            return false;
        }
    }

    if (parameters.CountArguments("platform") == 2)
    {
        commandInfo.buildPlatform = parameters.GetArguments("platform")[0];
        commandInfo.buildArchitecture = parameters.GetArguments("platform")[1];
    }

    if (parameters.Contains("debug"))
    {
        commandInfo.buildType = BuildType::Debug;
    }
    if (parameters.Contains("development"))
    {
        commandInfo.buildType = BuildType::Development;
    }
    if (parameters.Contains("release"))
    {
        commandInfo.buildType = BuildType::Release;
    }

    if (parameters.Contains("generate"))
    {
        if (commandInfo.projectPath.empty())
        {
            Utility::PrintLine("'-generate' requires a project to be specified");
            return false;
        }

        tasks.push_back(new TaskGenerate());
    }

    if (parameters.Contains("build"))
    {
        if (commandInfo.projectPath.empty())
        {
            Utility::PrintLine("'-build' requires a project to be specified");
            return false;
        }

        if (commandInfo.buildPlatform.length() == 0)
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return false;
        }

        if (commandInfo.buildType == BuildType::ENUMSIZE)
        {
            Utility::PrintLine("'-build' requires a build configuration to be specified");
            return false;
        }

        tasks.push_back(new TaskBuild(commandInfo.buildPlatform, commandInfo.buildArchitecture, commandInfo.buildType));
    }

    return true;
}

bool RunTasks(std::vector<ITask*>& tasks, CommandInfo& commandInfo)
{
    std::sort(tasks.begin(), tasks.end(), [](const ITask* A, const ITask* B) { return A->GetPriority() < B->GetPriority(); });

    if (!commandInfo.projectManifest.Parse(commandInfo.projectPath))
    {
        return false;
    }

    if (commandInfo.projectManifest.projectModules.empty())
    {
        Utility::PrintLine("Project does not contain any modules");
        return false;
    }

    for (ITask* task : tasks)
    {
        PerformanceTimer taskTimer;
        Utility::PrintLineD("Executing task: ");
        if (!task->Run(commandInfo.projectManifest)) return false;
        taskTimer.Stop("Task");
    }
    Utility::PrintLine("Done");

    return true;
}

int main(int argc, char** argv)
{
    PerformanceTimer::StartGlobalTimer();
    PerformanceTimer mainTimer;

    ParameterList parameters(argc, argv);

    std::vector<ITask*> tasks;

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    CommandInfo commandInfo;

    if (!ParseParameters(parameters, tasks, commandInfo))
    {
        return -1;
    }

    if (!tasks.empty())
    {
        if (!RunTasks(tasks, commandInfo))
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
