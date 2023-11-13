#include "GraphicsBufferVK.hpp"
#include "GraphicsAdapterVK.hpp"
#include "GraphicsSwapchainVK.hpp"
#include <Assert.hpp>

namespace Faro
{
    VkBufferUsageFlags Convert(GraphicsResourceType type)
    {
        switch (type)
        {
            case RT_ConstantBuffer:
            {
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            }
            case RT_VertexBuffer:
            {
                return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            }
            case RT_IndexBuffer:
            {
                return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            }
        }

        Debug_Break();
        return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }

    VkImageLayout Convert(GraphicsResourceState state)
    {
        switch (state)
        {
            case RS_Unknown: return VK_IMAGE_LAYOUT_UNDEFINED;
            case RS_CopySource: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case RS_CopyDestination: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RS_RenderTarget: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case RS_ShaderResource: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            //case RS_Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //TODO Support present layout
        }
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }

    void GraphicsBufferVK::Init()
    {
        IGraphicsAdapterChild::Init();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();

        const GraphicsBufferDesc& desc = GetDesc();
        if (desc.resourceType == RT_Texture)
        {
            if (desc.texture.renderTarget && desc.renderTarget.swapchain != nullptr)
            {
                GraphicsSwapchainVK* swapchain = (GraphicsSwapchainVK*)desc.renderTarget.swapchain;
                heapImage = swapchain->GetBackbufferImage(desc.renderTarget.swapchainImageIndex);
            }
            else
            {
                VkImageCreateInfo imageDesc = {};
                imageDesc.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                imageDesc.imageType = VK_IMAGE_TYPE_2D;
                imageDesc.extent.width = desc.texture.resolution.x;
                imageDesc.extent.height = desc.texture.resolution.y;
                imageDesc.extent.depth = 1;
                imageDesc.mipLevels = 1;
                imageDesc.arrayLayers = 1;
                imageDesc.format = VK_FORMAT_R8G8B8A8_UNORM; //TODO Convert format from desc
                imageDesc.tiling = VK_IMAGE_TILING_OPTIMAL;
                imageDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //TODO Set initial state from desc
                imageDesc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; //TODO Get flags from desc
                imageDesc.samples = VK_SAMPLE_COUNT_1_BIT;
                imageDesc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                vkCreateImage(adapter->GetDevice(), &imageDesc, nullptr, &heapImage);
                Debug_Assert(heapImage != nullptr);

                VkMemoryRequirements memoryDesc;
                vkGetImageMemoryRequirements(adapter->GetDevice(), heapImage, &memoryDesc);

                VkMemoryAllocateInfo allocDesc{};
                allocDesc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocDesc.allocationSize = memoryDesc.size;
                allocDesc.memoryTypeIndex = GetMemoryType();
                vkAllocateMemory(adapter->GetDevice(), &allocDesc, nullptr, &heapMemory);
                Debug_Assert(heapMemory != nullptr);

                vkBindImageMemory(adapter->GetDevice(), heapImage, heapMemory, 0);
            }
        }
        else 
        {
            VkBufferCreateInfo bufferCreateDesc = {};
            bufferCreateDesc.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateDesc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferCreateDesc.size = GetDesc().dataSize;
            bufferCreateDesc.usage = Convert(GetDesc().resourceType);
            vkCreateBuffer(adapter->GetDevice(), &bufferCreateDesc, nullptr, &heapBuffer);
            Debug_Assert(heapBuffer != nullptr);

            VkMemoryAllocateInfo memoryCreateDesc = {};
            memoryCreateDesc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            memoryCreateDesc.allocationSize = bufferCreateDesc.size;
            memoryCreateDesc.memoryTypeIndex = GetMemoryType();
            vkAllocateMemory(adapter->GetDevice(), &memoryCreateDesc, nullptr, &heapMemory);
            Debug_Assert(heapMemory != nullptr);

            vkBindBufferMemory(adapter->GetDevice(), heapBuffer, heapMemory, 0);
        }
    }

    void GraphicsBufferVK::Destroy()
    {
        if (heapBuffer != nullptr) 
        {
            vkDestroyBuffer(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), heapBuffer, nullptr);
            heapBuffer = nullptr;
        }

        if (heapImage != nullptr)
        {
            vkDestroyImage(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), heapImage, nullptr);
            heapImage = nullptr;
        }

        GraphicsBuffer::Destroy();
    }

    void GraphicsBufferVK::Upload(uint8* data)
    {
        //TODO Upload to heap
    }

    void GraphicsBufferVK::TransitionResource(VkCommandBuffer commandBuffer, GraphicsResourceState state)
    {
        const GraphicsBufferDesc& desc = GetDesc();
        if (desc.resourceType == RT_Texture)
        {
            VkImageMemoryBarrier barrierDesc = {};
            barrierDesc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

            barrierDesc.oldLayout = Convert(GetResourceState());
            barrierDesc.newLayout = Convert(state);
            barrierDesc.image = heapImage;
            barrierDesc.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
            barrierDesc.srcAccessMask = 0;
            barrierDesc.dstAccessMask = 0;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierDesc);
        }

        //TODO Implement buffer barrier

        SetResourceState(state);
    }

    uint32 GraphicsBufferVK::GetMemoryType()
    {
        return 0; //TODO Get index from adapter based on desc
    }
}
