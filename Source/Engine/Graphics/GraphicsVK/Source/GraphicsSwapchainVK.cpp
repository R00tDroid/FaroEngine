#include "GraphicsSwapchainVK.hpp"
#include <GraphicsAdapterVK.hpp>
#include <WindowWindows.hpp>

namespace Faro
{
    void GraphicsSwapchainVK::Init(GraphicsAdapter* adapter, Window* window)
    {
        GraphicsSwapchain::Init(adapter, window);

        WindowState windowState = window->GetWindowState();

        CreateBackbuffer();
    }

    void GraphicsSwapchainVK::Destroy()
    {
        GraphicsSwapchain::Destroy();
    }

    void GraphicsSwapchainVK::Present()
    {
    }
}
