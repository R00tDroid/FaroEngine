#include "GraphicsBufferVK.hpp"
#include "GraphicsAdapterVK.hpp"

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

        Debug_BreakMessage("Invalid heap type");
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
            imageDesc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageDesc.samples = VK_SAMPLE_COUNT_1_BIT;
            imageDesc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            vkCreateImage(adapter->GetDevice(), &imageDesc, nullptr, &heapImage);

            VkMemoryRequirements memoryDesc;
            vkGetImageMemoryRequirements(adapter->GetDevice(), heapImage, &memoryDesc);

            VkMemoryAllocateInfo allocDesc{};
            allocDesc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocDesc.allocationSize = memoryDesc.size;
            allocDesc.memoryTypeIndex = 0; //TODO Get correct memory type
            vkAllocateMemory(adapter->GetDevice(), &allocDesc, nullptr, &heapMemory);

            vkBindImageMemory(adapter->GetDevice(), heapImage, heapMemory, 0);
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
        //TODO Move transition logic to commandlist

        /*VkImageMemoryBarrier BarrierDesc = {};
        BarrierDesc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

        BarrierDesc.oldLayout = Convert(GetResourceState());
        BarrierDesc.newLayout = Convert(state);
        BarrierDesc.image = GetImage();
        BarrierDesc.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        BarrierDesc.srcAccessMask = 0;
        BarrierDesc.dstAccessMask = 0;

        vkCmdPipelineBarrier(CommandList->GetCommandBuffer(), VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &BarrierDesc);

        SetResourceState(state);*/
    }

    uint32 GraphicsBufferUploadVK::GetMemoryType()
    {
        return 0; //TODO Get type from adapter
    }

    uint32 GraphicsBufferRemoteVK::GetMemoryType()
    {
        return 0; //TODO Get type from adapter
    }
}
