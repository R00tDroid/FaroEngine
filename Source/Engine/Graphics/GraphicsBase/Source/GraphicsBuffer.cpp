#include "GraphicsBuffer.hpp"

namespace Faro
{
    const GraphicsHeapDesc& GraphicsBuffer::GetDesc()
    {
        return desc;
    }

    void GraphicsBuffer::Init(GraphicsAdapter* adapter, GraphicsCommandList*, GraphicsHeapDesc inDesc)
    {
        desc = inDesc;
        IGraphicsAdapterChild::Init(adapter);
    }
}
