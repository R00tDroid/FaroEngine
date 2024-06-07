#include "GraphicsSwapchainVK.hpp"
#include <GraphicsAdapterVK.hpp>
#include <WindowWindows.hpp>
#include <Assert.hpp>
#include "GraphicsInterfaceVK.hpp"
#include "GraphicsSemaphoreVK.hpp"

namespace Faro
{
    void GraphicsSwapchainVK::Init(GraphicsAdapter* adapter, Window* window)
    {
        GraphicsSwapchain::Init(adapter, window);

        if (!CreateSurface(window)) return;
        if (!CreateSwapchain()) return;
        if (!GetImages()) return;
        GetNextImageIndex();
        CreateBackbuffer();
    }

    void GraphicsSwapchainVK::Destroy()
    {
        if (swapchain != nullptr)
        {
            GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
            vkDestroySwapchainKHR(vkAdapter->GetDevice(), swapchain, nullptr);
            swapchain = nullptr;
        }

        if (windowSurface != nullptr)
        {
            GraphicsInterfaceVK* graphicsInterface = (GraphicsInterfaceVK*)GGraphics;
            vkDestroySurfaceKHR(graphicsInterface->GetInstance(), windowSurface, nullptr);
            windowSurface = nullptr;
        }

        GraphicsSwapchain::Destroy();
    }

    void GraphicsSwapchainVK::Present()
    {
        //TODO setup semaphore before presenting
        VkPresentInfoKHR presentDesc = {};
        presentDesc.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentDesc.swapchainCount = 1;
        presentDesc.pSwapchains = &swapchain;
        presentDesc.pImageIndices = &currentImage;

        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        vkQueuePresentKHR(vkAdapter->GetQueue(), &presentDesc);

        GetNextImageIndex();
    }

    VkImage GraphicsSwapchainVK::GetBackbufferImage(uint32 index)
    {
        if (images.IsValidIndex(index))
        {
            return images[index];
        }

        return nullptr;
    }

    bool GraphicsSwapchainVK::CreateSurface(Window* window)
    {
        WindowWindows* nativeWindow = (WindowWindows*)window;

        GraphicsInterfaceVK* graphicsInterface = (GraphicsInterfaceVK*)GGraphics;

        VkWin32SurfaceCreateInfoKHR surfaceDesc = {};
        surfaceDesc.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceDesc.hwnd = nativeWindow->GetHandle();
        surfaceDesc.hinstance = GetModuleHandle(nullptr);
        vkCreateWin32SurfaceKHR(graphicsInterface->GetInstance(), &surfaceDesc, nullptr, &windowSurface);

        return windowSurface != nullptr;
    }

    bool GraphicsSwapchainVK::CreateSwapchain()
    {
        VkSurfaceCapabilitiesKHR capabilities;

        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkAdapter->GetPhysicalDevice(), windowSurface, &capabilities);

        VkSurfaceFormatKHR format = SelectFormat();
        Debug_Assert(format.format != VK_FORMAT_UNDEFINED);

        VkPresentModeKHR presentMode = SelectPresentMode();
        Debug_Assert(presentMode != VK_PRESENT_MODE_MAX_ENUM_KHR);

        VkSwapchainCreateInfoKHR swapchainDesc = {};
        swapchainDesc.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainDesc.surface = windowSurface;
        swapchainDesc.minImageCount = 2;
        swapchainDesc.imageFormat = format.format;
        swapchainDesc.imageColorSpace = format.colorSpace;
        swapchainDesc.imageExtent = capabilities.currentExtent;
        swapchainDesc.imageArrayLayers = 1;
        swapchainDesc.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainDesc.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainDesc.preTransform = capabilities.currentTransform;
        swapchainDesc.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainDesc.presentMode = presentMode;
        swapchainDesc.clipped = VK_TRUE;

        vkCreateSwapchainKHR(vkAdapter->GetDevice(), &swapchainDesc, nullptr, &swapchain);

        return swapchain != nullptr;
    }

    bool GraphicsSwapchainVK::GetImages()
    {
        Debug_Assert(images.Empty());

        uint32 enumCount;
        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        vkGetSwapchainImagesKHR(vkAdapter->GetDevice(), swapchain, &enumCount, nullptr);

        Debug_Assert(enumCount == 2);

        images.Resize(enumCount);
        vkGetSwapchainImagesKHR(vkAdapter->GetDevice(), swapchain, &enumCount, images.Data());

        return true;
    }

    VkSurfaceFormatKHR GraphicsSwapchainVK::SelectFormat()
    {
        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        Array<VkSurfaceFormatKHR> formats;
        uint32 enumCount;

        vkGetPhysicalDeviceSurfaceFormatsKHR(vkAdapter->GetPhysicalDevice(), windowSurface, &enumCount, nullptr);
        formats.Resize(enumCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkAdapter->GetPhysicalDevice(), windowSurface, &enumCount, formats.Data());

        for (VkSurfaceFormatKHR& format : formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                return format;
            }
        }

        return { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR };
    }

    VkPresentModeKHR GraphicsSwapchainVK::SelectPresentMode()
    {
        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        Array<VkPresentModeKHR> presentModes;
        uint32 enumCount;

        vkGetPhysicalDeviceSurfacePresentModesKHR(vkAdapter->GetPhysicalDevice(), windowSurface, &enumCount, nullptr);
        presentModes.Resize(enumCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkAdapter->GetPhysicalDevice(), windowSurface, &enumCount, presentModes.Data());

        if (presentModes.Contains(VK_PRESENT_MODE_MAILBOX_KHR)) return VK_PRESENT_MODE_MAILBOX_KHR;
        if (presentModes.Contains(VK_PRESENT_MODE_FIFO_KHR)) return VK_PRESENT_MODE_FIFO_KHR;

        return VK_PRESENT_MODE_MAX_ENUM_KHR;
    }

    void GraphicsSwapchainVK::GetNextImageIndex()
    {
        GraphicsSemaphoreVK* syncPoint = static_cast<GraphicsSemaphoreVK*>(GetSyncPoint(0)); //TODO Use image index.

        GraphicsAdapterVK* vkAdapter = GetTypedAdapter<GraphicsAdapterVK>();
        vkAcquireNextImageKHR(vkAdapter->GetDevice(), swapchain, UINT64_MAX, syncPoint->GetHandle(), nullptr, &currentImage);
    }
}
