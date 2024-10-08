#if FARO_OS_WINDOWS
#include "GraphicsAdapterD3D12.hpp"
#include <Memory/MemoryManager.hpp>
#include <GraphicsLogD3D12.hpp>
#include <GraphicsCommandListD3D12.hpp>
#include <GraphicsBufferD3D12.hpp>
#include <GraphicsFenceD3D12.hpp>
#include <GraphicsSwapchainD3D12.hpp>
#include <GraphicsPipelineD3D12.hpp>

namespace Faro
{
    void GraphicsAdapterD3D12::Init(GraphicsAdapterDesc& inDesc)
    {
        desc = inDesc;

        Logger::Log(GraphicsLogD3D12, LC_Info, "Creating adapter: %s", desc.name.Data());

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

        Logger::Log(GraphicsLogD3D12, LC_Trace, "Create command queue");
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
    }

    void GraphicsAdapterD3D12::Destroy()
    {
        Logger::Log(GraphicsLogD3D12, LC_Info, "Destroy adapter: %s", desc.name.Data());

        commandQueue->Release();
        device->Release();
        dxgiAdapter->Release();
    }

    ID3D12Device2* GraphicsAdapterD3D12::GetDevice()
    {
        return device;
    }

    ID3D12CommandQueue* GraphicsAdapterD3D12::GetCommandQueue()
    {
        return commandQueue;
    }
    GraphicsCommandList* GraphicsAdapterD3D12::CreateCommandList()
    {
        GraphicsCommandListD3D12* commandList = MemoryManager::New<GraphicsCommandListD3D12>();
        commandList->Init(this);
        return commandList;
    }

    GraphicsBuffer* GraphicsAdapterD3D12::CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc)
    {
        GraphicsBuffer* buffer = nullptr;

        switch (type)
        {
            case BT_Upload: { buffer = MemoryManager::New<GraphicsBufferUploadD3D12>(); }
            case BT_Remote: { buffer = MemoryManager::New<GraphicsBufferRemoteD3D12>(); }
        }

        if (buffer != nullptr)
        {
            buffer->Init(this, desc);
        }

        return buffer;
    }

    GraphicsFence* GraphicsAdapterD3D12::CreateFence()
    {
        GraphicsFenceD3D12* fence = MemoryManager::New<GraphicsFenceD3D12>();
        fence->Init(this);
        return fence;
    }

    GraphicsSwapchain* GraphicsAdapterD3D12::CreateSwapchain(Window* window)
    {
        GraphicsSwapchainD3D12* swapchain = MemoryManager::New<GraphicsSwapchainD3D12>();
        swapchain->Init(this, window);
        return swapchain;
    }

    GraphicsPipeline* GraphicsAdapterD3D12::CreatePipeline(GraphicsPipelineDesc desc)
    {
        GraphicsPipelineD3D12* pipeline = MemoryManager::New<GraphicsPipelineD3D12>();
        pipeline->Init(this, desc);
        return pipeline;
    }
}
#endif
