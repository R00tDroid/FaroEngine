
#include "Commandline.hpp"
#include "Tasks/TaskBuild.hpp"
#include "Tasks/TaskGenerate.hpp"

void PrintHelp()
{
    Utility::Print("Faro Engine build tool\n\n");
    Utility::Print("Tasks:\n");
    Utility::Print("-build    Compile the specified source\n");
    Utility::Print("-clean        Clean intermediates\n");
    Utility::Print("-generate    Generate Visual Studio project\n");
    Utility::Print("-deploy        Assemble build for specified project\n\n");

    Utility::Print("Compilation: -compile\n");
    Utility::Print("-project <path>                                        The project to compile\n");
    Utility::Print("-module <module1, module2>                            Modules to be compile\n");
    Utility::Print("-debug / -development / -release                        The configuration to compile with (debug / deployment / release)\n");
    Utility::Print("-platform <platform type><architecture>             Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("Cleaning: -clean\n");
    Utility::Print("-project <path>                                        The project to clean\n");
    Utility::Print("-module <module1, module2>                            Modules to be cleaned\n");
    Utility::Print("-debug / -development / -release                    The configuration to build or clean (debug / development / release)\n");
    Utility::Print("-platform <platform type> <architecture> [flavor]    Defines the platform (windows, android or linux) and architecture (x86, x64, arm, arm-v7, arm64, mips)\n\n");

    Utility::Print("solution generation: -generate\n");
    Utility::Print("-project <path>    The project to generate solution files for\n");
}

bool ParseProject(ParameterList& parameters, BuilderConfiguration& config)
{
    if (parameters.Contains("project"))
    {
        if (parameters.HasArguments("project"))
        {
            try
            {
                std::filesystem::path path = std::filesystem::weakly_canonical(parameters.GetArguments("project")[0]);
                path.make_preferred();
                config.projectManifestPath = Utility::CopyString(path.string());
            }
            catch (std::exception const& e)
            {
                Utility::PrintLine("Failed to locate project: " + parameters.GetArguments("project")[0] + " (" + e.what() + ")");
                return false;
            }

            Utility::PrintLineD("Project argument: " + std::string(config.projectManifestPath));
        }
        else
        {
            Utility::PrintLine("Invalid -project argument. See -help for more information.");
            return false;
        }
    }

    return true;
}

bool ParsePlatform(ParameterList& parameters, BuilderConfiguration& config)
{
    if (parameters.Contains("platform"))
    {
        if (parameters.CountArguments("platform") == 1)
        {
            config.build.buildPlatform = Utility::CopyString(parameters.GetArguments("platform")[0]);
            Utility::PrintLineD("Platform argument: " + std::string(config.build.buildPlatform));
        }
        else
        {
            Utility::PrintLine("Invalid -platform argument. See -help for more information.");
            return false;
        }
    }

    return true;
}

bool ParseBuildType(ParameterList& parameters, BuilderConfiguration& config)
{
    if (parameters.Contains("debug"))
    {
        config.build.buildType = BuildType::Debug;
        Utility::PrintLineD("Build type argument: debug");
    }
    if (parameters.Contains("development"))
    {
        config.build.buildType = BuildType::Development;
        Utility::PrintLineD("Build type argument: development");
    }
    if (parameters.Contains("release"))
    {
        config.build.buildType = BuildType::Release;
        Utility::PrintLineD("Build type argument: release");
    }

    return true;
}

bool ParseModule(ParameterList& parameters, BuilderConfiguration& config)
{
    if (parameters.HasArguments("module"))
    {
        config.modulesCount = parameters.CountArguments("module");

        if (config.modulesCount > 0) 
        {
            config.modules = new const char* [config.modulesCount];

            for (unsigned int i = 0; i < config.modulesCount; i++)
            {
                config.modules[i] = Utility::CopyString(parameters.GetArguments("module")[i]);
            }
        }

        Utility::PrintLineD("Module argument: " + std::to_string(config.modulesCount));
    }

    return true;
}

bool ParseTasks(ParameterList& parameters, BuilderConfiguration& config)
{
    if (parameters.Contains("generate"))
    {
        Utility::PrintLineD("Generate argument");

        if (config.projectManifestPath == nullptr)
        {
            Utility::PrintLine("'-generate' requires a project to be specified");
            return false;
        }

        config.runGeneration = true;
    }

    if (parameters.Contains("build"))
    {
        Utility::PrintLineD("Build argument");

        if (config.projectManifestPath == nullptr)
        {
            Utility::PrintLine("'-build' requires a project to be specified");
            return false;
        }

        if (config.build.buildPlatform == nullptr)
        {
            Utility::PrintLine("'-build' requires a platform to be specified");
            return false;
        }

        if (config.build.buildType == BuildType::ENUMSIZE)
        {
            Utility::PrintLine("'-build' requires a build configuration to be specified");
            return false;
        }

        config.runBuild = true;
    }

    return true;
}

bool ParseParameters(ParameterList& parameters, BuilderConfiguration& config)
{
    if (!ParseProject(parameters, config)) return false;
    if (!ParsePlatform(parameters, config)) return false;
    if (!ParseBuildType(parameters, config)) return false;
    if (!ParseModule(parameters, config)) return false;
    if (!ParseTasks(parameters, config)) return false;

    return true;
}
