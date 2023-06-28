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

    void GraphicsBufferVK::Init()
    {
        IGraphicsAdapterChild::Init();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();

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

    void GraphicsBufferVK::Destroy()
    {
        if (heapBuffer != nullptr) 
        {
            vkDestroyBuffer(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), heapBuffer, nullptr);
            heapBuffer = nullptr;
        }
        GraphicsBuffer::Destroy();
    }

    void GraphicsBufferVK::Upload(uint8* data)
    {
        //TODO Upload to heap
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
