#include "Commandline.hpp"
#include "Version.generated.hpp"
#include "Tasks/TaskBuild.hpp"
#include "Tasks/TaskGenerate.hpp"

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

bool ParseParameters(ParameterList& parameters, std::vector<ITask*>& tasks, TaskRunInfo& runInfo)
{
    if (parameters.HasArguments("project"))
    {
        try
        {
            runInfo.projectManifestPath = std::filesystem::weakly_canonical(parameters.GetArguments("project")[0]);
            runInfo.projectManifestPath.make_preferred();
        }
        catch (std::exception const& e)
        {
            Utility::PrintLine("Failed to locate project: " + parameters.GetArguments("project")[0] + " (" + e.what() + ")");
            return false;
        }

        Utility::PrintLineD("Project argument: " + runInfo.projectManifestPath.string());
    }

    if (parameters.CountArguments("platform") == 2)
    {
        runInfo.buildPlatform = parameters.GetArguments("platform")[0];
        runInfo.buildArchitecture = parameters.GetArguments("platform")[1];

        Utility::PrintLineD("Platform argument: " + runInfo.buildPlatform + ", " + runInfo.buildArchitecture);
    }

    if (parameters.Contains("debug"))
    {
        runInfo.buildType = BuildType::Debug;
        Utility::PrintLineD("Build type argument: debug");
    }
    if (parameters.Contains("development"))
    {
        runInfo.buildType = BuildType::Development;
        Utility::PrintLineD("Build type argument: development");
    }
    if (parameters.Contains("release"))
    {
        runInfo.buildType = BuildType::Release;
        Utility::PrintLineD("Build type argument: release");
    }

    if (parameters.HasArguments("module"))
    {
        for (int i = 0; i < parameters.CountArguments("module"); i++)
        {
            runInfo.moduleList.push_back(parameters.GetArguments("module")[i]);
        }

        Utility::PrintLineD("Module argument: " + std::to_string(runInfo.moduleList.size()));
    }

    if (parameters.Contains("generate"))
    {
        Utility::PrintLineD("Generate argument");

        if (runInfo.projectManifestPath.empty())
        {
            Utility::PrintLine("'-generate' requires a project to be specified");
            return false;
        }

        tasks.push_back(new TaskGenerate());
    }

    if (parameters.Contains("build"))
    {
        Utility::PrintLineD("Build argument");

        if (runInfo.projectManifestPath.empty())
        {
            Utility::PrintLine("'-build' requires a project to be specified");
            return false;
        }

        if (runInfo.buildPlatform.length() == 0)
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return false;
        }

        if (runInfo.buildType == BuildType::ENUMSIZE)
        {
            Utility::PrintLine("'-build' requires a build configuration to be specified");
            return false;
        }

        tasks.push_back(new TaskBuild(runInfo.buildPlatform, runInfo.buildArchitecture, runInfo.buildType));
    }

    return true;
}
