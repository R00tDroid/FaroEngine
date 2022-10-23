#pragma once
#include <GraphicsInterface.hpp>
#include <dxgi1_4.h>

namespace Faro
{
    class GraphicsInterfaceD3D12 : public GraphicsInterface
    {
    public:
        bool Init() override;
        void Destroy() override;

        String GetName() override { return "D3D12"; }

        Array<GraphicsAdapterDesc> GetAdapters() override;

        GraphicsAdapter* CreateAdapter(GraphicsAdapterDesc description) override;

        IDXGIFactory4* GetFactory();

    private:
        void DetectAdapters();
        
    private:
        IDXGIFactory4* dxgiFactory = nullptr;
        Array<GraphicsAdapterDesc> adapterDescs;
    };

    REGISTER_GRAPHICS(GraphicsInterfaceD3D12)
}
