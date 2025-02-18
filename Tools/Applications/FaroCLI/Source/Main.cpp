#include "Parameters.hpp"
#include "Utility.hpp"
#include "Commandline.hpp"
#include "ProjectInfo.hpp"

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
        //TODO Find correct setup
        BuildSetup setup;
        if (!project.load(setup))
        {
            Utility::PrintLine("Error while loading project");
            return -1;
        }
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
