#pragma once
#include <Module.hpp>

namespace Faro
{
    class ModuleManager
    {
    public:
        ModuleManager();
        ~ModuleManager();

        bool Init();
        bool Update();
        bool Destroy();

    private:
        Array<IModule*> modules;
    };

    extern ModuleManager* GModuleManager;
}
