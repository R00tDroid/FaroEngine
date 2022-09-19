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
    };

    REGISTER_GRAPHICS(GraphicsInterfaceD3D12)
}
