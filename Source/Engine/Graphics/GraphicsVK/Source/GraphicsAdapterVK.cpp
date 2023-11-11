#include "GraphicsAdapterVK.hpp"
#include <Assert.hpp>
#include "GraphicsBufferVK.hpp"
#include "GraphicsCommandListVK.hpp"
#include "GraphicsFenceVK.hpp"
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

        // Find memory types
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        Map<VkMemoryPropertyFlagBits, String> flagNames;
        flagNames.Add(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "DeviceLocal");
        flagNames.Add(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "HostVisible");
        flagNames.Add(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, "hostCoherent");
        flagNames.Add(VK_MEMORY_PROPERTY_HOST_CACHED_BIT, "HostCached");

        for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            VkMemoryType& type = memoryProperties.memoryTypes[i];

            String flags = "";

            if (type.propertyFlags == 0)
            {
                flags = " None";
            }
            else 
            {
                for (auto& it : flagNames)
                {
                    if ((type.propertyFlags & it.first) == it.first)
                    {
                        flags += " " + it.second;
                    }
                }
            }

            Logger::Log(GraphicsLogVK, LC_Debug, "Memory type: %i, Flags: %s, Heap index %i, Heap size %imb", i, flags.Data(), type.heapIndex, memoryProperties.memoryHeaps[type.heapIndex].size / 1024 / 1024);
        }
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
        GraphicsBuffer* buffer = MemoryManager::New<GraphicsBufferVK>();
        buffer->Init(this, desc);
        return buffer;
    }

    GraphicsFence* GraphicsAdapterVK::CreateFence()
    {
        GraphicsFenceVK* fence = MemoryManager::New<GraphicsFenceVK>();
        fence->Init(this);
        return fence;
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

    VkQueue GraphicsAdapterVK::GetQueue()
    {
        return queue;
    }

    uint32 GraphicsAdapterVK::GetQueueIndex()
    {
        return graphicsQueue;
    }
}
