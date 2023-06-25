#include "GraphicsCommandListVK.hpp"

#include "GraphicsAdapterVK.hpp"
#include "GraphicsBufferVK.hpp"

namespace Faro
{
    void GraphicsCommandListVK::Init()
    {
        GraphicsCommandList::Init();
    }

    void GraphicsCommandListVK::Destroy()
    {
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
    }

    void GraphicsCommandListVK::CopyBuffer(GraphicsBuffer* sourcePtr, GraphicsBuffer* destinationPtr)
    {
    }

    void GraphicsCommandListVK::ClearRenderTarget(GraphicsBuffer* inRenderTarget, FloatColor color)
    {
    }
}
