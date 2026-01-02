#include "Parameters.hpp"
#include "Utility.hpp"
#include "Commandline.hpp"
#include "Generator.hpp"
#include "ProjectInfo.hpp"
#include "Toolchain.hpp"
#include "Builder.hpp"

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

            if (taskInfo.runClean)
            {
                //TODO Pass specific modules
                if (!Builder::clean(setup, &project, 0, nullptr))
                {
                    Utility::PrintLine("Error while cleaning");
                    return -1;
                }
            }

            if (taskInfo.runBuild)
            {
                //TODO Pass specific modules
                if (!Builder::build(setup, &project, 0, nullptr))
                {
                    Utility::PrintLine("Error while building");
                    return -1;
                }
            }

            //TODO Run deploy
        }

        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
