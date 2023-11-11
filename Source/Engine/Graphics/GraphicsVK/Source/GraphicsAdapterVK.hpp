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

        VkQueue GetQueue();

        uint32 GetQueueIndex();

        uint32 GetMemoryIndexRemote();

        uint32 GetMemoryIndexUpload();

    private:
        static int32 GetMemoryIndex(const VkPhysicalDeviceMemoryProperties& memoryProperties, VkMemoryPropertyFlags memoryFlags);

        VkDevice device = nullptr;
        VkQueue queue = nullptr;
        uint32 graphicsQueue = 0;

        int32 memoryIndexRemote = -1;
        int32 memoryIndexUpload = -1;
    };
}
