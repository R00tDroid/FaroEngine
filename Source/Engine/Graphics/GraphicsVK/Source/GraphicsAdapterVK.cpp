#include "GraphicsAdapterVK.hpp"
#include "GraphicsCommandListVK.hpp"
#include "GraphicsInterface.hpp"
#include "GraphicsInterfaceVK.hpp"
#include "GraphicsLogVK.hpp"

namespace Faro
{
    void GraphicsAdapterVK::Init(GraphicsAdapterDesc& inDesc)
    {
        Logger::Log(GraphicsLogVK, LC_Info, "Creating adapter: %s", inDesc.name.Data());

        VkPhysicalDevice physicalDevice = (VkPhysicalDevice)inDesc.payload;
        Debug_Assert(physicalDevice != nullptr);

        // Find graphics queue index
        uint32_t queueTypeCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, nullptr);

        Array<VkQueueFamilyProperties> queueType;
        queueType.Resize(uint16(queueTypeCount));
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, queueType.Data());

        VkDeviceQueueCreateInfo queueCreateDesc = {};
        queueCreateDesc.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        float QueuePriority = 1.0f;
        queueCreateDesc.pQueuePriorities = &QueuePriority;

        for (uint32 i = 0; i < queueType.Size(); i++)
        {
            if ((queueType[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT && 
                (queueType[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
            {
                graphicsQueue = i;
                queueCreateDesc.queueFamilyIndex = graphicsQueue;
                queueCreateDesc.queueCount = 1;
                break;
            }
        }

        if (queueCreateDesc.queueCount == 0)
        {
            Logger::Log(GraphicsLogVK, LC_Error, "Failed to create graphics queue");
            return;
        }

        VkDeviceCreateInfo deviceCreateDesc = {};
        deviceCreateDesc.pQueueCreateInfos = &queueCreateDesc;
        deviceCreateDesc.queueCreateInfoCount = 1;
        deviceCreateDesc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Create device
        if (vkCreateDevice(physicalDevice, &deviceCreateDesc, nullptr, &device) != VK_SUCCESS)
        {
            Logger::Log(GraphicsLogVK, LC_Error, "Failed to create device");
            return;
        }
        Debug_Assert(device != nullptr);

        // Load function pointers
        int32 gladVersion = gladLoaderLoadVulkan(static_cast<GraphicsInterfaceVK*>(GGraphics)->GetInstance(), physicalDevice, device);
        Logger::Log(GraphicsLogVK, LC_Debug, "GLAD loader: %i.%i", GLAD_VERSION_MAJOR(gladVersion), GLAD_VERSION_MINOR(gladVersion));
        Debug_Assert(gladVersion != 0);

        // Query queue
        vkGetDeviceQueue(device, queueCreateDesc.queueFamilyIndex, 0, &queue);
        Debug_Assert(queue != nullptr);
    }

    void GraphicsAdapterVK::Destroy()
    {
        if (device != nullptr)
        {
            vkDestroyDevice(device, nullptr);
            device = nullptr;
            queue = nullptr;
        }
    }

    GraphicsCommandList* GraphicsAdapterVK::CreateCommandList()
    {
        GraphicsCommandListVK* commandList = MemoryManager::New<GraphicsCommandListVK>();
        commandList->Init(this);
        return commandList;
    }

    GraphicsBuffer* GraphicsAdapterVK::CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc)
    {
        return nullptr;
    }

    GraphicsFence* GraphicsAdapterVK::CreateFence()
    {
        return nullptr;
    }

    GraphicsSwapchain* GraphicsAdapterVK::CreateSwapchain(Window* window)
    {
        return nullptr;
    }

    GraphicsPipeline* GraphicsAdapterVK::CreatePipeline(GraphicsPipelineDesc desc)
    {
        return nullptr;
    }

    VkDevice GraphicsAdapterVK::GetDevice()
    {
        return device;
    }

    uint32 GraphicsAdapterVK::GetGraphicsQueue()
    {
        return graphicsQueue;
    }
}
