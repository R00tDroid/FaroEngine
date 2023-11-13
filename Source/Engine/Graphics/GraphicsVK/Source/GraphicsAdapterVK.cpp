#include "GraphicsAdapterVK.hpp"
#include <Assert.hpp>
#include "GraphicsBufferVK.hpp"
#include "GraphicsCommandListVK.hpp"
#include "GraphicsFenceVK.hpp"
#include "GraphicsInterface.hpp"
#include "GraphicsInterfaceVK.hpp"
#include "GraphicsLogVK.hpp"
#include "GraphicsPipelineVK.hpp"
#include "GraphicsSwapchainVK.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    Faro::Logger::Log(Faro::GraphicsLogVK, Faro::LC_Debug, "%s", pCallbackData->pMessage);

    if ((severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ||
        (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        Debug_Break();
    }

    return VK_FALSE;
}

namespace Faro
{
    void GraphicsAdapterVK::Init(GraphicsAdapterDesc& inDesc)
    {
        Logger::Log(GraphicsLogVK, LC_Info, "Creating adapter: %s", inDesc.name.Data());

        physicalDevice = (VkPhysicalDevice)inDesc.payload;
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

        if (!VerifyDeviceExtensions(physicalDevice))
        {
            return;
        }

        VkDeviceCreateInfo deviceCreateDesc = {};
        deviceCreateDesc.pQueueCreateInfos = &queueCreateDesc;
        deviceCreateDesc.queueCreateInfoCount = 1;
        deviceCreateDesc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        Array<char*> extensions = GetDeviceExtensions();
        deviceCreateDesc.enabledExtensionCount = extensions.Size();
        deviceCreateDesc.ppEnabledExtensionNames = extensions.Data();

        // Create device
        if (vkCreateDevice(physicalDevice, &deviceCreateDesc, nullptr, &device) != VK_SUCCESS)
        {
            Logger::Log(GraphicsLogVK, LC_Error, "Failed to create device");
            return;
        }
        Debug_Assert(device != nullptr);

        // Load function pointers
        int32 gladVersion = gladLoaderLoadVulkan(static_cast<GraphicsInterfaceVK*>(GGraphics)->GetInstance(), physicalDevice, device);
        Logger::Log(GraphicsLogVK, LC_Debug, "GLAD device: %i.%i", GLAD_VERSION_MAJOR(gladVersion), GLAD_VERSION_MINOR(gladVersion));
        Debug_Assert(gladVersion != 0);

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugDesc = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugDesc.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugDesc.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugDesc.pfnUserCallback = debugCallback;
        vkCreateDebugUtilsMessengerEXT(static_cast<GraphicsInterfaceVK*>(GGraphics)->GetInstance(), &debugDesc, nullptr, &debugMessenger);
#endif

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

        memoryIndexRemote = GetMemoryIndex(memoryProperties, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        Debug_Assert(memoryIndexRemote >= 0);
        memoryIndexUpload = GetMemoryIndex(memoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        Debug_Assert(memoryIndexUpload >= 0);
    }

    void GraphicsAdapterVK::Destroy()
    {
#ifndef NDEBUG
        if (debugMessenger != nullptr)
        {
            vkDestroyDebugUtilsMessengerEXT(static_cast<GraphicsInterfaceVK*>(GGraphics)->GetInstance(), debugMessenger, nullptr);
            debugMessenger = nullptr;
        }
#endif

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
        GraphicsSwapchainVK* swapchain = MemoryManager::New<GraphicsSwapchainVK>();
        swapchain->Init(this, window);
        return swapchain;
    }

    GraphicsPipeline* GraphicsAdapterVK::CreatePipeline(GraphicsPipelineDesc desc)
    {
        GraphicsPipelineVK* pipeline = MemoryManager::New<GraphicsPipelineVK>();
        pipeline->Init(this, desc);
        return pipeline;
    }

    VkPhysicalDevice GraphicsAdapterVK::GetPhysicalDevice()
    {
        return physicalDevice;
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

    uint32 GraphicsAdapterVK::GetMemoryIndexRemote()
    {
        return memoryIndexRemote;
    }

    uint32 GraphicsAdapterVK::GetMemoryIndexUpload()
    {
        return memoryIndexUpload;
    }

    int32 GraphicsAdapterVK::GetMemoryIndex(const VkPhysicalDeviceMemoryProperties& memoryProperties, VkMemoryPropertyFlags memoryFlags)
    {
        for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            const VkMemoryType& type = memoryProperties.memoryTypes[i];
            if (type.propertyFlags == memoryFlags)
            {
                return i;
            }
        }

        return -1;
    }

    Array<char*> GraphicsAdapterVK::GetDeviceExtensions()
    {
        return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    }

    bool GraphicsAdapterVK::VerifyDeviceExtensions(VkPhysicalDevice physicalDevice)
    {
        uint32 extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

        Array<VkExtensionProperties> availableExtensions;
        availableExtensions.Resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.Data());

        Array<String> missingExtensions;
        Array<char*> extensions = GetDeviceExtensions();
        for (char* extension : extensions)
        {
            missingExtensions.Add(extension);
        }

        for (VkExtensionProperties& extension : availableExtensions)
        {
            if (missingExtensions.Contains(extension.extensionName))
            {
                missingExtensions.Remove(extension.extensionName);
            }
        }

        if (!missingExtensions.Empty())
        {
            for (String& extension : missingExtensions)
            {
                Logger::Log(GraphicsLogVK, LC_Error, "Missing device extension: %s", extension.Data());
            }
            return false;
        }

        return true;
    }
}
