#include "GraphicsAdapterVK.hpp"

namespace Faro
{
    void GraphicsAdapterVK::Init(GraphicsAdapterDesc& inDesc)
    {
    }

    void GraphicsAdapterVK::Destroy()
    {
    }

    GraphicsCommandList* GraphicsAdapterVK::CreateCommandList()
    {
        return nullptr;
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
}
