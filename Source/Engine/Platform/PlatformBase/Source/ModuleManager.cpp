#include "ModuleManager.h"
#include <Module.h>

namespace Faro
{
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

        for (IModule* module : modules)
        {
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
        for (IModule* module : modules)
        {
            module->Unload();
        }
        return true;
    }
}
