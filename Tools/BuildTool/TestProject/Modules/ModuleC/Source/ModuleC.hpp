#pragma once
#include <Module.hpp>
#include <Platform.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    class ModuleC : public IModule
    {
    public:
        void Load() override;
        void Unload() override;
        String GetName() override;
        Array<String> GetRuntimeDependencies() override;
    };

    REGISTER_MODULE(ModuleC)
}
