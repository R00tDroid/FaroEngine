#include "GraphicsAdapterD3D12.hpp"

namespace Faro
{
    void GraphicsAdapterD3D12::Init(GraphicsAdapterDesc& desc)
    {
        dxgiAdapter = static_cast<IDXGIAdapter1*>(desc.payload);

        D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    }

    void GraphicsAdapterD3D12::Destroy()
    {
        commandQueue->Release();
        device->Release();
        dxgiAdapter->Release();
    }
}
