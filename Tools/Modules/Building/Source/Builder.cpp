#include "Builder.hpp"
#include <map>
#include "BuilderTasks.hpp"
#include "ModuleInfo.hpp"
#include "ProjectInfo.hpp"
#include "Utility.hpp"
#include "Worker.hpp"

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

    std::vector<std::vector<const ModuleManifest*>> buildStages;
    for (auto& it : moduleOrder)
    {
        const ModuleManifest* module = it.first;
        unsigned int& order = it.second;

        if (order >= buildStages.size()) buildStages.resize(order + 1);
        buildStages[order].push_back(module);
    }

    WorkerPool workerPool;
    workerPool.start();

    BuilderInfo builderInfo = { workerPool };

    for (size_t stageId = 0; stageId < buildStages.size(); stageId++)
    {
        const std::vector<const ModuleManifest*>& buildStage = buildStages[stageId];
        Utility::PrintLineD("Build stage " + std::to_string(stageId) + ": " + std::to_string(buildStage.size()));

        for (const ModuleManifest* module : buildStage)
        {
            workerPool.addTask<ModuleCheckTask>(builderInfo, module);
        }

        //TODO Schedule tasks
        workerPool.waitForCompletion();
    }

    workerPool.stop();

    return false;
}
