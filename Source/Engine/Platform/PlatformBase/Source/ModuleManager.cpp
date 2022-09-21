#include "ModuleManager.hpp"
#include <Module.hpp>
#include <Log.hpp>
#include <Memory/MemoryManager.hpp>
#include <Math/MathUtil.hpp>

namespace Faro
{
    LOG_DECLARATION(ModuleManagerLog, ModuleManager)

    ModuleManager moduleManagerInstance;
    ModuleManager* GModuleManager;

    ModuleManager::ModuleManager()
    {
        GModuleManager = this;
    }

    ModuleManager::~ModuleManager()
    {
        GModuleManager = nullptr;
    }

    bool ModuleManager::Init()
    {
        modules = GetRegisteredModules();

        if (!SortModules()) return false;

        Log(ModuleManagerLog, LC_Debug, "Loading modules");
        for (IModule* module : modules)
        {
            Log(ModuleManagerLog, LC_Debug, "Loading module: %s", module->GetName().Data());
            module->Load();
        }
    
        return true;
    }

    bool ModuleManager::Update()
    {
        return true;
    }

    bool ModuleManager::Destroy()
    {
        Log(ModuleManagerLog, LC_Debug, "Unloading modules");
        for (IModule* module : modules)
        {
            module->Unload();
        }
        return true;
    }

    struct ModuleDependencies
    {
        IModule* module = nullptr;
        Array<ModuleDependencies*> upstream;
        Array<ModuleDependencies*> downstream;
    };

    void ResolveModuleDependencies(ModuleDependencies* moduleInfo)
    {
        for (ModuleDependencies* dependency : moduleInfo->upstream)
        {
            dependency->module->loadOrder = Max(dependency->module->loadOrder, moduleInfo->module->loadOrder + 1);
            ResolveModuleDependencies(dependency);
        }
    }

    bool ModuleManager::SortModules()
    {
        Map<IModule*, ModuleDependencies*> dependencies;
        Map<String, ModuleDependencies*> moduleNames;

        // Prepare module info
        for (IModule* module : modules) 
        {
            ModuleDependencies* dependencyInfo = MemoryManager::New<ModuleDependencies>();
            dependencyInfo->module = module;
            dependencies.Add(module, dependencyInfo);
            moduleNames.Add(module->GetName(), dependencyInfo);
        }

        // Resolve dependencies
        for (auto& it : dependencies)
        {
            for (const String& dependencyName : it.first->GetRuntimeDependencies())
            {
                if (!moduleNames.Contains(dependencyName))
                {
                    Log(ModuleManagerLog, LC_Error, "Missing module runtime dependency: %s > %s", it.first->GetName().Data(), dependencyName.Data());
                    return false;
                }

                ModuleDependencies* dependency = moduleNames[dependencyName];
                dependency->upstream.Add(it.second);
                it.second->downstream.Add(dependency);
            }
        }

        // Determine load order
        for (auto& it : dependencies)
        {
            if (it.second->downstream.Empty())
            {
                ResolveModuleDependencies(it.second);
            }
        }

        modules.Sort([dependencies](IModule* a, IModule* b)
        {
            return a->loadOrder < b->loadOrder;
        });

        // Cleanup module info
        for (auto& it : dependencies)
        {
            MemoryManager::Delete(it.second);
        }

        return true;
    }
}
