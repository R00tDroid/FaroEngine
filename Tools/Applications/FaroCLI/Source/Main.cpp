#include "Parameters.hpp"
#include "Utility.hpp"
#include "Commandline.hpp"
#include "Generator.hpp"
#include "ProjectInfo.hpp"
#include "Toolchain.hpp"
#include "Builder.hpp"

bool findBuildSetup(TaskInfo& taskInfo, BuildSetup& setup)
{
    setup.target = nullptr;
    setup.configuration = taskInfo.config;

    std::string requestedTarget = taskInfo.platformName;
    std::transform(requestedTarget.begin(), requestedTarget.end(), requestedTarget.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);
            std::string targetId = target->identifier();
            std::transform(targetId.begin(), targetId.end(), targetId.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });

            if (requestedTarget == targetId)
            {
                setup.target = target;
                break;
            }
        }
    }

    if (setup.target == nullptr)
    {
        Utility::PrintLine("Unable to find target " + taskInfo.platformName);
        return false;
    }

    return true;
}

bool runTasks(TaskInfo& taskInfo, ProjectManifest& project, BuildSetup& setup)
{
    if (taskInfo.runClean)
    {
        std::vector<ModuleManifest*> modules;
        if (!taskInfo.modules.empty())
        {
            if (!project.findModules(taskInfo.modules, modules))
            {
                return false;
            }
        }
        ModuleManifest** modulesPtr = modules.empty() ? nullptr : modules.data();
        if (!Builder::clean(setup, &project, static_cast<unsigned int>(modules.size()), const_cast<const ModuleManifest**>(modulesPtr)))
        {
            Utility::PrintLine("Error while cleaning");
            return false;
        }
    }

    if (taskInfo.runBuild)
    {
        std::vector<ModuleManifest*> modules;
        if (!taskInfo.modules.empty())
        {
            if (!project.findModules(taskInfo.modules, modules))
            {
                return false;
            }
        }
        ModuleManifest** modulesPtr = modules.empty() ? nullptr : modules.data();
        if (!Builder::build(setup, &project, static_cast<unsigned int>(modules.size()), const_cast<const ModuleManifest**>(modulesPtr)))
        {
            Utility::PrintLine("Error while building");
            return false;
        }
    }

    //TODO Run deploy

    return true;
}

int main(int argc, char** argv)
{
    ParameterList parameters(argc, const_cast<const char**>(argv));

    if (parameters.Contains("help"))
    {
        printHelp();
        return 0;
    }

    TaskInfo taskInfo;

    if (!parseParameters(parameters, taskInfo))
    {
        return -1;
    }

    ProjectManifest project(taskInfo.manifest.string().c_str());

    if (taskInfo.runConfigure ||
        taskInfo.runProject ||
        taskInfo.runBuild ||
        taskInfo.runClean ||
        taskInfo.runDeploy)
    {
        if (taskInfo.runConfigure)
        {
            if (!project.configure())
            {
                Utility::PrintLine("Error while configuring project");
                return -1;
            }
        }

        if (taskInfo.runProject ||
            taskInfo.runBuild ||
            taskInfo.runClean ||
            taskInfo.runDeploy)
        {
            BuildSetup setup;
            if (!findBuildSetup(taskInfo, setup))
            {
                return -1;
            }

            if (!project.load(setup))
            {
                Utility::PrintLine("Error while loading project");
                return -1;
            }

            if (taskInfo.runProject)
            {
                if (!Generator::generate(&project))
                {
                    Utility::PrintLine("Error while generating a project ");
                    return -1;
                }
            }

            if (!runTasks(taskInfo, project, setup)) {
                return -1;
            }
        }

        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
