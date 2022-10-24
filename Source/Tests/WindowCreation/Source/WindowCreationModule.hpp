#pragma once
#include <Module.hpp>
#include <Platform.hpp>
#include <GraphicsAdapter.hpp>

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

        GraphicsAdapter* adapter = nullptr;
        GraphicsFence* fence = nullptr;
        GraphicsSwapchain* swapchain = nullptr;
        GraphicsCommandList* commandList = nullptr;
        GraphicsBuffer* uploadBuffer = nullptr;
        GraphicsBuffer* remoteBuffer = nullptr;
    };

    REGISTER_MODULE(WindowCreationModule)
}
