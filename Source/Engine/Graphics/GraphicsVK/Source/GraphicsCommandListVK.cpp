#include "GraphicsCommandListVK.hpp"
#include "GraphicsAdapterVK.hpp"
#include "GraphicsBufferVK.hpp"
#include "GraphicsLogVK.hpp"
#include <Assert.hpp>

namespace Faro
{
    void GraphicsCommandListVK::Init()
    {
        GraphicsCommandList::Init();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();

        VkCommandPoolCreateInfo poolCreateDesc = {};
        poolCreateDesc.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateDesc.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateDesc.queueFamilyIndex = adapter->GetQueueIndex();

        if (vkCreateCommandPool(adapter->GetDevice(), &poolCreateDesc, nullptr, &commandPool) != VK_SUCCESS)
        {
            Logger::Log(GraphicsLogVK, LC_Error, "Failed to create command pool");
            return;
        }
        Debug_Assert(commandPool != nullptr);

        VkCommandBufferAllocateInfo bufferCreateDesc = {};
        bufferCreateDesc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        bufferCreateDesc.commandPool = commandPool;
        bufferCreateDesc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufferCreateDesc.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(adapter->GetDevice(), &bufferCreateDesc, &commandBuffer) != VK_SUCCESS)
        {
            Logger::Log(GraphicsLogVK, LC_Error, "Failed to create command buffer failed");
            return;
        }
        Debug_Assert(commandBuffer != nullptr);

        vkEndCommandBuffer(commandBuffer);
    }

    void GraphicsCommandListVK::Destroy()
    {
        if (commandPool != nullptr)
        {
            vkDestroyCommandPool(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), commandPool, nullptr);
            commandPool = nullptr;
        }

        GraphicsCommandList::Destroy();
    }

    void GraphicsCommandListVK::Reset()
    {
    }

    void GraphicsCommandListVK::Execute()
    {
    }

    void GraphicsCommandListVK::SetResourceState(GraphicsBuffer* bufferPtr, GraphicsResourceState state)
    {
        if (bufferPtr->GetResourceState() == state) return;

        GraphicsBufferVK* buffer = static_cast<GraphicsBufferVK*>(bufferPtr);
        buffer->TransitionResource(commandBuffer, state);
    }

    void GraphicsCommandListVK::CopyBuffer(GraphicsBuffer* sourcePtr, GraphicsBuffer* destinationPtr)
    {
    }

    void GraphicsCommandListVK::ClearRenderTarget(GraphicsBuffer* inRenderTarget, FloatColor color)
    {
    }
}
