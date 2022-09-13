#pragma once

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
    };

    extern ModuleManager* GModuleManager;
}
