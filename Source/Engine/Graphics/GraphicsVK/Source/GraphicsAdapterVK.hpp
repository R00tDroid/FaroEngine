#pragma once
#include <GraphicsAdapter.hpp>
#include <glad/vulkan.h>

namespace Faro
{
    class GraphicsAdapterVK : public GraphicsAdapter
    {
    public:
        void Init(GraphicsAdapterDesc&) override;
        void Destroy() override;

        GraphicsCommandList* CreateCommandList() override;

        GraphicsBuffer* CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc) override;

        GraphicsFence* CreateFence() override;

        GraphicsSwapchain* CreateSwapchain(Window* window) override;

        GraphicsPipeline* CreatePipeline(GraphicsPipelineDesc desc) override;

        VkDevice GetDevice();

        uint32 GetGraphicsQueue();

    private:
        VkDevice device = nullptr;
        VkQueue queue = nullptr;
        uint32 graphicsQueue = 0;
    };
}
