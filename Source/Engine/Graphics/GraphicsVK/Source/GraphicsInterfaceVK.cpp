#include "GraphicsInterfaceVK.hpp"
#include <ShaderDefinition.hpp>
#include "GraphicsAdapterVK.hpp"
#include "GraphicsLogVK.hpp"

namespace Faro
{
    bool GraphicsInterfaceVK::Init()
    {
        int32 gladVersion = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
        Logger::Log(GraphicsLogVK, LC_Debug, "GLAD instance: %i.%i", GLAD_VERSION_MAJOR(gladVersion), GLAD_VERSION_MINOR(gladVersion));

        if (!VerifyInstanceExtensions()) return false;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Wyvern Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;

        Array<String> availableLayers = GetAvailableLayers();
        Array<char*> enabledLayers;

#ifndef NDEBUG
        if (availableLayers.Contains("VK_LAYER_KHRONOS_validation"))
        {
            Logger::Log(GraphicsLogVK, LC_Debug, "Enabled validation layer");
            enabledLayers.Add("VK_LAYER_KHRONOS_validation");
        }
#endif

        if (!enabledLayers.Empty())
        {
            instanceInfo.enabledLayerCount = enabledLayers.Size();
            instanceInfo.ppEnabledLayerNames = enabledLayers.Data();
        }

        Array<char*> enabledExtensions = GetInstanceExtensions();
        instanceInfo.enabledExtensionCount = enabledExtensions.Size();
        instanceInfo.ppEnabledExtensionNames = enabledExtensions.Data();

        vkCreateInstance(&instanceInfo, nullptr, &instance);

        uint32 devices;
        vkEnumeratePhysicalDevices(instance, &devices, nullptr);

        if (devices > 0)
        {
            Array<VkPhysicalDevice> deviceList;
            deviceList.Resize(uint16(devices));
            vkEnumeratePhysicalDevices(instance, &devices, deviceList.Data());

            for (VkPhysicalDevice& device : deviceList)
            {
                VkPhysicalDeviceProperties adapterProperties = {};
                vkGetPhysicalDeviceProperties(device, &adapterProperties);

                GraphicsAdapterDesc adapterDesc = {};
                adapterDesc.payload = device;
                adapterDesc.name = adapterProperties.deviceName;
                adapterDesc.manufacturer = VendorCodeToString(adapterProperties.vendorID);

                VkPhysicalDeviceMemoryProperties memoryProperties = {};
                vkGetPhysicalDeviceMemoryProperties(device, &memoryProperties);
                
                for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
                {
                    VkMemoryType& type = memoryProperties.memoryTypes[i];

                    if (type.propertyFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
                    {
                        adapterDesc.vramDedicated = memoryProperties.memoryHeaps[type.heapIndex].size;
                    }
                    else if (type.propertyFlags == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
                    {
                        adapterDesc.vramShared = memoryProperties.memoryHeaps[type.heapIndex].size;
                    }
                }
                
                adapterDesc.vramTotal = adapterDesc.vramDedicated + adapterDesc.vramShared;

                adapters.Add(adapterDesc);
            }
        }

        return true;
    }

    void GraphicsInterfaceVK::Destroy()
    {
        if (instance != nullptr)
        {
            vkDestroyInstance(instance, nullptr);
            instance = nullptr;
        }
    }

    Array<GraphicsAdapterDesc> GraphicsInterfaceVK::GetAdapters()
    {
        return adapters;
    }

    GraphicsAdapter* GraphicsInterfaceVK::CreateAdapter(GraphicsAdapterDesc description)
    {
        GraphicsAdapterVK* adapter = MemoryManager::New<GraphicsAdapterVK>();
        adapter->Init(description);
        return adapter;
    }

    uint8 GraphicsInterfaceVK::GetShaderBinaryType()
    {
        return ShaderBinaryType_SPRIV;
    }

    VkInstance GraphicsInterfaceVK::GetInstance()
    {
        return instance;
    }

    Array<char*> GraphicsInterfaceVK::GetInstanceExtensions()
    {
        return {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifndef NDEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };
    }

    bool GraphicsInterfaceVK::VerifyInstanceExtensions()
    {
        uint32 extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        Array<VkExtensionProperties> availableExtensions;
        availableExtensions.Resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.Data());

        Array<String> missingExtensions;
        Array<char*> extensions = GetInstanceExtensions();
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
                Logger::Log(GraphicsLogVK, LC_Error, "Missing instance extension: %s", extension.Data());
            }
            return false;
        }

        return true;
    }

    Array<String> GraphicsInterfaceVK::GetAvailableLayers()
    {
        uint32_t count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);

        Array<VkLayerProperties> layers;
        layers.Resize(uint16(count));
        vkEnumerateInstanceLayerProperties(&count, layers.Data());

        Array<String> layerNames;
        for (VkLayerProperties& layer : layers)
        {
            layerNames.Add(layer.layerName);
        }

        return layerNames;
    }
}
