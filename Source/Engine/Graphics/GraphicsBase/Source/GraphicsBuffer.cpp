#include "GraphicsBuffer.hpp"

namespace Faro
{
    const GraphicsBufferDesc& GraphicsBuffer::GetDesc()
    {
        return desc;
    }

    void GraphicsBuffer::Init(GraphicsAdapter* adapter, GraphicsBufferDesc inDesc)
    {
        desc = inDesc;
        IGraphicsAdapterChild::Init(adapter);
    }
}
