#pragma once
#include <Module.hpp>
#include <ModuleA.hpp>

namespace Faro
{
    class ModuleB : public IModule
    {
    public:
        void Load() override;
        void Unload() override;
        String GetName() override;
        Array<String> GetRuntimeDependencies() override;
    };

    REGISTER_MODULE(ModuleB)
}
