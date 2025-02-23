#include "Builder.hpp"
#include "ProjectInfo.hpp"
#include "Utility.hpp"

bool Builder::build(const BuildSetup& buildSetup, const ProjectManifest* project, unsigned int moduleCount, const ModuleManifest** moduleList)
{
    if (buildSetup.target == nullptr)
    {
        Utility::PrintLine("Invalid build target");
        return false;
    }

    const Toolchain* toolchain = buildSetup.target->toolchain();
    if (toolchain == nullptr)
    {
        Utility::PrintLine("Invalid toolchain for build target");
        return false;
    }

    std::vector<const ModuleManifest*> modules;
    if (moduleCount == 0)
    {
        for (unsigned int i = 0; i < project->modules(); i++)
        {
            modules.push_back(project->module(i));
        }
    }
    else
    {
        for (unsigned int i = 0; i < moduleCount; i++)
        {
            modules.push_back(moduleList[i]);
        }
    }

    return false;
}
