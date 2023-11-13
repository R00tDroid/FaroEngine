#pragma once
#include <GraphicsSwapchain.hpp>
#include "Vulkan.hpp"

namespace Faro
{
    class GraphicsSwapchainVK : public GraphicsSwapchain
    {
    public:
        void Init(GraphicsAdapter* adapter, Window* window) override;
        void Destroy() override;

        void Present() override;

    private:
        bool CreateSurface(Window* window);
        bool CreateSwapchain();

        VkSurfaceFormatKHR SelectFormat();
        VkPresentModeKHR SelectPresentMode();

        VkSurfaceKHR windowSurface = nullptr;
        VkSwapchainKHR swapchain = nullptr;
    };
}
