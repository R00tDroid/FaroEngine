#pragma once
#include <GraphicsAdapter.hpp>
#include "Vulkan.hpp"

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
        uint32 GetMemoryType();

        VkDeviceMemory heapMemory = nullptr;
        VkBuffer heapBuffer = nullptr;
        VkImage heapImage = nullptr;
    };
}
