#include "GraphicsAdapterD3D12.hpp"

namespace Faro
{
    void GraphicsAdapterD3D12::Init(GraphicsAdapterDesc& desc)
    {
        dxgiAdapter = static_cast<IDXGIAdapter1*>(desc.payload);

        D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

        ID3D12InfoQueue* infoQueue = nullptr;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
        {
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

            infoQueue->Release();
        }

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
