#include "GraphicsCommandListVK.hpp"
#include "GraphicsAdapterVK.hpp"
#include "GraphicsBufferVK.hpp"
#include "GraphicsLogVK.hpp"
#include <Assert.hpp>
#include "Math/MathUtil.hpp"
#include "GraphicsSemaphoreVK.hpp"

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
        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo beginDesc = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(commandBuffer, &beginDesc);    
    }

#define MaxWaitSemaphores 16

    void GraphicsCommandListVK::Execute(Array<GraphicsSemaphore*> waitForSemaphores)
    {
        VkSemaphore waitSemaphoreHandles[MaxWaitSemaphores];
        for (uint32 i = 0; i < Min<uint32>(MaxWaitSemaphores, waitForSemaphores.Size()); i++)
        {
            waitSemaphoreHandles[i] = static_cast<GraphicsSemaphoreVK*>(waitForSemaphores[i])->GetHandle();
        }

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pCommandBuffers = &commandBuffer;
        info.commandBufferCount = 1;
        info.pWaitSemaphores = waitSemaphoreHandles;
        info.waitSemaphoreCount = waitForSemaphores.Size();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();
        vkQueueSubmit(adapter->GetQueue(), 1, &info, nullptr);
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
