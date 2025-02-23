#include "Builder.hpp"
#include <map>
#include "ModuleInfo.hpp"
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

    std::map<const ModuleManifest*, unsigned int> moduleOrder;
    for (const ModuleManifest* module : modules) 
    {
        moduleOrder.insert(std::pair<const ModuleManifest*, unsigned int>(module, 0));
    }

    //TODO Detect circular dependencies
    while (true)
    {
        bool anyMoved = false;

        for (auto& it : moduleOrder)
        {
            const ModuleManifest* module = it.first;
            unsigned int& order = it.second;

            for (unsigned int dependencyIndex = 0; dependencyIndex < module->dependencies(); dependencyIndex++)
            {
                const ModuleManifest* dependency = module->dependency(dependencyIndex);
                auto dependencyIt = moduleOrder.find(dependency);
                if (dependencyIt != moduleOrder.end())
                {
                    const unsigned int& dependencyOrder = dependencyIt->second;
                    if (order <= dependencyOrder)
                    {
                        order = dependencyOrder + 1;
                        anyMoved = true;
                    }
                }
            }
        }

        if (!anyMoved) break;
    }

    return false;
}
