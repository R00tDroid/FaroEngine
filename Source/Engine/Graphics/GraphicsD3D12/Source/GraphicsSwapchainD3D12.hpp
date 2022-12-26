#pragma once
#include <GraphicsSwapchain.hpp>
#include <dxgi1_5.h>

namespace Faro
{
    class GraphicsSwapchainD3D12 : public GraphicsSwapchain
    {
    public:
        void Init(GraphicsAdapter* adapter, Window* window) override;
        void Destroy() override;

        IDXGISwapChain4* GetNativeSwapchain();

        void Present() override;

    private:
        IDXGISwapChain4* swapchain = nullptr;
    };
}
