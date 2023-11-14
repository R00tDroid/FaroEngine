#include "GraphicsBuffer.hpp"

namespace Faro
{
    const GraphicsBufferDesc& GraphicsBuffer::GetDesc()
    {
        return desc;
    }

    GraphicsBufferType GraphicsBuffer::GetBufferType()
    {
        return type;
    }

    void GraphicsBuffer::Init(GraphicsAdapter* adapter, GraphicsBufferCreateDesc createDesc)
    {
        desc = createDesc.bufferDesc;
        type = createDesc.bufferType;
        IGraphicsAdapterChild::Init(adapter);
    }

    GraphicsResourceState GraphicsBuffer::GetResourceState()
    {
        return state;
    }

    void GraphicsBuffer::SetResourceState(GraphicsResourceState inState)
    {
        state = inState;
    }
}
