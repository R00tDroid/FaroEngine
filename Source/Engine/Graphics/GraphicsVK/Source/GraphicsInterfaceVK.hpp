#pragma once
#include <GraphicsInterface.hpp>
#include <glad/vulkan.h>

namespace Faro
{
    class GraphicsInterfaceVK : public GraphicsInterface
    {
    public:
        bool Init() override;
        void Destroy() override;

        String GetName() override { return "Vulkan"; }

        Array<GraphicsAdapterDesc> GetAdapters() override;

        GraphicsAdapter* CreateAdapter(GraphicsAdapterDesc description) override;

        uint8 GetShaderBinaryType() override;

        VkInstance GetInstance();

    private:
        Array<String> GetAvailableLayers();

        Array<GraphicsAdapterDesc> adapters;
        VkInstance instance = nullptr;
    };

    REGISTER_GRAPHICS(GraphicsInterfaceVK)
}
