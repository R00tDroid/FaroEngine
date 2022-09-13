#include "ModuleManager.h"

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
        return true;
    }

    bool ModuleManager::Update()
    {
        return true;
    }

    bool ModuleManager::Destroy()
    {
        return true;
    }
}
