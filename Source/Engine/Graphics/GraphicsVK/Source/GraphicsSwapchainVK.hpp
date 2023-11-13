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

        VkImage GetBackbufferImage(uint32 index);

    private:
        bool CreateSurface(Window* window);
        bool CreateSwapchain();
        bool GetImages();

        VkSurfaceFormatKHR SelectFormat();
        VkPresentModeKHR SelectPresentMode();

        VkSurfaceKHR windowSurface = nullptr;
        VkSwapchainKHR swapchain = nullptr;
        Array<VkImage> images;
    };
}
