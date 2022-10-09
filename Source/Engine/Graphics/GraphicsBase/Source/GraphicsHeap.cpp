#include "GraphicsHeap.hpp"

namespace Faro
{
    const GraphicsHeapDesc& GraphicsHeap::GetDesc()
    {
        return desc;
    }

    void GraphicsHeap::Init(GraphicsAdapter* adapter, GraphicsCommandList*, GraphicsHeapDesc inDesc)
    {
        desc = inDesc;
        IGraphicsAdapterChild::Init(adapter);
    }
}
