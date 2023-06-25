#pragma once
#include <GraphicsInterface.hpp>

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
    };

    REGISTER_GRAPHICS(GraphicsInterfaceVK)
}
