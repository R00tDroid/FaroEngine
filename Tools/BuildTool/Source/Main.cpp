#include <vector>
#include "Parameters.hpp"
#include "ProjectManifest.hpp"
#include "Utility.hpp"
#include "Version.generated.hpp"
#include "Tasks/ITask.hpp"
#include "Tasks/TaskGenerate.hpp"

int main(int argc, char** argv)
{
    PerformanceTimer::StartGlobalTimer();
    PerformanceTimer mainTimer;

    ParameterList parameters(argc, argv);

    std::vector<ITask*> tasks;

    if (parameters.Contains("help"))
    {
        Utility::Print("Faro Engine build tool " + std::string(EngineVersion) + "\n\n");
        Utility::Print("Tasks:\n");
        Utility::Print("-build	Compile the specified source\n");
        Utility::Print("-clean		Clean intermediates\n");
        Utility::Print("-generate	Generate Visual Studio project\n");
        Utility::Print("-deploy		Assemble build for specified project\n\n");

        Utility::Print("Compilation: -compile\n");
        Utility::Print("-project <path>										The project to compile\n");
        Utility::Print("-module <module1, module2>							Modules to be compile\n");
        Utility::Print("-debug / -development / -release						The configuration to compile with (debug / deployment / release)\n");
        Utility::Print("-platform <platform type> <architecture>         	Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

        Utility::Print("Cleaning: -clean\n");
        Utility::Print("-project <path>										The project to clean\n");
        Utility::Print("-module <module1, module2>							Modules to be cleaned\n");
        Utility::Print("-debug / -release									The configuration to clean (debug / release)\n");
        Utility::Print("-platform <platform type> <architecture> [flavor]	Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

        Utility::Print("solution generation: -generate\n");
        Utility::Print("-project <path>	The project to generate solution files for\n");

        return 0;
    }

    std::string projectPath = "";
    ProjectManifest projectManifest;

    std::string buildPlatform = "";
    std::string buildArchitecture = "";

    if (parameters.HasArguments("project"))
    {
        //TODO sanity checks
        projectPath = parameters.GetArguments("project")[0];
    }

    if (parameters.CountArguments("platform") == 2)
    {
        buildPlatform = parameters.GetArguments("platform")[0];
        buildArchitecture = parameters.GetArguments("platform")[1];
    }

    if (parameters.Contains("generate"))
    {
        if (projectPath.length() <= 0)
        {
            Utility::PrintLine("'-generate' requires a project to be specified");
            return -1;
        }

        tasks.push_back(new TaskGenerate());
    }

    if (parameters.Contains("build"))
    {
        if (projectPath.length() <= 0)
        {
            Utility::PrintLine("'-build' requires a project to be specified");
            return -1;
        }

        if (buildPlatform.length() == 0) 
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return -1;
        }
        else
        {
            //tasks.push_back(new TaskBuild(buildPlatform, buildArchitecture));
        }
    }

    if (!tasks.empty())
    {
        //tasks.Sort((a, b) => b.GetPriority() - a.GetPriority());

        if (!projectManifest.Parse(projectPath))
        {
            return -1;
        }

        if (projectManifest.projectModules.empty())
        {
            Utility::PrintLine("Project does not contain any modules");
            return -1;
        }

        for (ITask* task : tasks)
        {
            PerformanceTimer taskTimer;
            Utility::PrintLineD("Executing task: ");
            if (!task->Run(projectManifest)) return -1;
            taskTimer.Stop("Task: ");
        }
        Utility::PrintLine("Done");

        mainTimer.Stop("Main");

#if DEBUG
        PerformanceTimer.PrintTimers();
#endif

        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
