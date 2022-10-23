#pragma once
#include <GraphicsAdapter.hpp>
#include <d3d12.h>
#include <dxgi.h>

namespace Faro
{
    class GraphicsAdapterD3D12 : public GraphicsAdapter
    {
    public:
        void Init(GraphicsAdapterDesc&) override;
        void Destroy() override;

        ID3D12Device2* GetDevice();

        ID3D12CommandQueue* GetCommandQueue();

        GraphicsCommandList* CreateCommandList() override;

        GraphicsBuffer* CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc) override;

        GraphicsFence* CreateFence() override;

        GraphicsSwapchain* CreateSwapchain(Window* window) override;

    private:
        GraphicsAdapterDesc desc;

        IDXGIAdapter1* dxgiAdapter = nullptr;
        ID3D12Device2* device = nullptr;

        ID3D12CommandQueue* commandQueue = nullptr;
    };
}
