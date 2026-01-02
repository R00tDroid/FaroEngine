#include "Commandline.hpp"
#include "Version.generated.hpp"

void printHelp()
{
    Utility::Print("Faro Engine build tool " + std::string(EngineVersion) + "\n\n");
    Utility::Print("Tasks:\n");
    Utility::Print("-configure    Generate project files\n");
    Utility::Print("-project    Generate Visual Studio project\n");
    Utility::Print("-build    Compile the specified source\n");
    Utility::Print("-clean        Clean intermediates\n");
    Utility::Print("-deploy        Assemble build for specified project\n\n");

    Utility::Print("Building: -compile\n");
    Utility::Print("-path <path>                                        The project to build\n");
    Utility::Print("-module <module1, module2>                            Modules to be build\n");
    Utility::Print("-debug / -development / -release                        The configuration to compile with (debug / deployment / release)\n");
    Utility::Print("-platform <platform type><architecture>             Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("Cleaning: -clean\n");
    Utility::Print("-path <path>                                        The project to clean\n");
    Utility::Print("-module <module1, module2>                            Modules to be cleaned\n");
    Utility::Print("-debug / -development / -release                    The configuration to build or clean (debug / development / release)\n");
    Utility::Print("-platform <platform type> <architecture> [flavor]    Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("solution generation: -generate\n");
    Utility::Print("-path <path>    The project to generate solution files for\n");
}

bool parseProject(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (parameters.Contains("path"))
    {
        if (parameters.HasArguments("path"))
        {
            try
            {
                taskInfo.manifest = std::filesystem::weakly_canonical(parameters.GetArguments("path")[0]);
                taskInfo.manifest.make_preferred();
            }
            catch (std::exception const& e)
            {
                Utility::PrintLine("Failed to locate project: " + parameters.GetArguments("path")[0] + " (" + e.what() + ")");
                return false;
            }

            Utility::PrintLineD("Path argument: " + taskInfo.manifest.string());
        }
        else
        {
            Utility::PrintLine("Invalid -path argument. See -help for more information.");
            return false;
        }
    }

    return true;
}

bool parsePlatform(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (parameters.Contains("platform"))
    {
        if (parameters.CountArguments("platform") == 1)
        {
            taskInfo.platformName = parameters.GetArguments("platform")[0];
            Utility::PrintLineD("Platform argument: " + taskInfo.platformName);
        }
        else
        {
            Utility::PrintLine("Invalid -platform argument. See -help for more information.");
            return false;
        }
    }

    return true;
}

bool parseBuildType(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (parameters.Contains("debug"))
    {
        taskInfo.config = C_Debug;
        Utility::PrintLineD("Build type argument: debug");
    }
    if (parameters.Contains("development"))
    {
        taskInfo.config = C_Development;
        Utility::PrintLineD("Build type argument: development");
    }
    if (parameters.Contains("release"))
    {
        taskInfo.config = C_Release;
        Utility::PrintLineD("Build type argument: release");
    }

    return true;
}

bool parseModule(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (parameters.HasArguments("module"))
    {
        for (int i = 0; i < parameters.CountArguments("module"); i++)
        {
            taskInfo.modules.push_back(parameters.GetArguments("module")[i]);
        }

        Utility::PrintLineD("Module argument: " + std::to_string(taskInfo.modules.size()));
    }

    return true;
}

bool parseTasks(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (parameters.Contains("configure"))
    {
        Utility::PrintLineD("Configure argument");
        taskInfo.runConfigure = true;
    }

    if (parameters.Contains("project"))
    {
        Utility::PrintLineD("Project argument");
        taskInfo.runProject = true;
    }

    if (parameters.Contains("build"))
    {
        Utility::PrintLineD("Build argument");
        taskInfo.runBuild = true;
    }

    if (parameters.Contains("clean"))
    {
        Utility::PrintLineD("Clean argument");
        taskInfo.runClean = true;
    }

    return true;
}

bool verifyTasks(TaskInfo& taskInfo)
{
    if (taskInfo.runConfigure)
    {
        if (taskInfo.manifest.empty())
        {
            Utility::PrintLine("'-configure' requires a project to be specified");
            return false;
        }
    }

    if (taskInfo.runProject)
    {
        if (taskInfo.manifest.empty())
        {
            Utility::PrintLine("'-project' requires a project to be specified");
            return false;
        }
    }

    if (taskInfo.runBuild)
    {
        if (taskInfo.manifest.empty())
        {
            Utility::PrintLine("'-build' requires a project to be specified");
            return false;
        }

        if (taskInfo.platformName.empty())
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return false;
        }

        if (taskInfo.config == C_ENUMSIZE)
        {
            Utility::PrintLine("'-build' requires a build configuration to be specified");
            return false;
        }
    }

    return true;
}

bool parseParameters(ParameterList& parameters, TaskInfo& taskInfo)
{
    if (!parseProject(parameters, taskInfo)) return false;
    if (!parsePlatform(parameters, taskInfo)) return false;
    if (!parseBuildType(parameters, taskInfo)) return false;
    if (!parseModule(parameters, taskInfo)) return false;
    if (!parseTasks(parameters, taskInfo)) return false;

    if (!verifyTasks(taskInfo)) return false;

    return true;
}
