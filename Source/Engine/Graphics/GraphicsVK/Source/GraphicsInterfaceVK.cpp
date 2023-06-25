#include "GraphicsInterfaceVK.hpp"
#include <ShaderDefinition.hpp>
#include "GraphicsLogVK.hpp"

namespace Faro
{
    bool GraphicsInterfaceVK::Init()
    {
        int32 gladVersion = gladLoaderLoadVulkan(nullptr, nullptr, nullptr);
        Logger::Log(GraphicsLogVK, LC_Debug, "GLAD loader: %i.%i.%i", VK_API_VERSION_MAJOR(gladVersion), VK_API_VERSION_MINOR(gladVersion), VK_API_VERSION_PATCH(gladVersion));

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Wyvern Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo{};
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

        vkCreateInstance(&instanceInfo, nullptr, &instance);

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
        return {};
    }

    GraphicsAdapter* GraphicsInterfaceVK::CreateAdapter(GraphicsAdapterDesc description)
    {
        return nullptr;
    }

    uint8 GraphicsInterfaceVK::GetShaderBinaryType()
    {
        return ShaderBinaryType_SPRIV;
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
