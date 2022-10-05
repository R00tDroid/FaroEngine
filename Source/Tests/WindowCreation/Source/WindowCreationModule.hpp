#pragma once
#include <Module.hpp>
#include <Platform.hpp>

namespace Faro
{
    class WindowCreationModule : public IModule
    {
    public:
        void Load() override;
        void Unload() override;
        String GetName() override;
        Array<String> GetRuntimeDependencies() override;

    private:
        Window* window = nullptr;
    };

    REGISTER_MODULE(WindowCreationModule)
}
