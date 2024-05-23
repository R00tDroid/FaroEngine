#if FARO_OS_WINDOWS
#include "GraphicsSwapchainD3D12.hpp"
#include <GraphicsAdapterD3D12.hpp>
#include <WindowWindows.hpp>

#include "GraphicsInterface.hpp"
#include "GraphicsInterfaceD3D12.hpp"

namespace Faro
{
    void GraphicsSwapchainD3D12::Init(GraphicsAdapter* adapter, Window* window)
    {
        GraphicsSwapchain::Init(adapter, window);

        WindowState windowState = window->GetWindowState();

        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.Width = windowState.innerSize.x;
        desc.Height = windowState.innerSize.y;
        desc.BufferCount = 2;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.SampleDesc.Count = 1;

        IDXGIFactory2* factory = static_cast<GraphicsInterfaceD3D12*>(GGraphics)->GetFactory();
        factory->CreateSwapChainForHwnd(GetTypedAdapter<GraphicsAdapterD3D12>()->GetCommandQueue(), static_cast<WindowWindows*>(window)->GetHandle() , &desc, NULL, NULL, (IDXGISwapChain1**)&swapchain);

        CreateBackbuffer();
    }

    void GraphicsSwapchainD3D12::Destroy()
    {
        swapchain->Release();
        swapchain = nullptr;

        GraphicsSwapchain::Destroy();
    }

    IDXGISwapChain4* GraphicsSwapchainD3D12::GetNativeSwapchain()
    {
        return swapchain;
    }

    void GraphicsSwapchainD3D12::Present()
    {
        DXGI_PRESENT_PARAMETERS presentParams = { };
        swapchain->Present1(0, 0, &presentParams);
    }
}
#endif
