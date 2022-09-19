#pragma once
#include <GraphicsInterface.hpp>

namespace Faro
{
    class GraphicsInterfaceD3D12 : public GraphicsInterface
    {
    public:
        bool Init() override;
        void Destroy() override;

        String GetName() override { return "D3D12"; }

        Array<GraphicsAdapterDesc> GetAdapters() override;
    };

    REGISTER_GRAPHICS(GraphicsInterfaceD3D12)
}
