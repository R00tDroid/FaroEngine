#pragma once
#include <Module.hpp>

namespace Faro
{
    class WindowCreationModule : public IModule
    {
    public:
        void Load() override;
        void Unload() override;
        String GetName() override;
        Array<String> GetRuntimeDependencies() override;
    };

    REGISTER_MODULE(WindowCreationModule)
}
