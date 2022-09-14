#include "ModuleManager.hpp"
#include <Module.hpp>
#include <Log.hpp>

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

        //TODO sort modules based on dependencies

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
}
