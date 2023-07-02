#pragma once
#include <GraphicsAdapter.hpp>
#include <glad/vulkan.h>

namespace Faro
{
    class GraphicsBufferVK : public GraphicsBuffer
    {
    public:
        void Init() override;

        void Destroy() override;

        void Upload(uint8* data) override;

        void TransitionResource(VkCommandBuffer commandBuffer, GraphicsResourceState state);

    protected:
        virtual uint32 GetMemoryType() = 0;

        VkDeviceMemory heapMemory = nullptr;
        VkBuffer heapBuffer = nullptr;
    };

    class GraphicsBufferUploadVK : public GraphicsBufferVK
    {
    protected:
        uint32 GetMemoryType() override;
    };

    class GraphicsBufferRemoteVK : public GraphicsBufferVK
    {
    protected:
        uint32 GetMemoryType() override;
    };
}
