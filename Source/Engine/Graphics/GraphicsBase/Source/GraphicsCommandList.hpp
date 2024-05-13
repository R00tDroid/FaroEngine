#pragma once
#include <GraphicsAdapterChild.hpp>
#include <GraphicsBuffer.hpp>
#include <Math/Color.hpp>
#include <GraphicsSemaphore.hpp>

namespace Faro
{
    class GraphicsCommandList : public IGraphicsAdapterChild
    {
    public:
        virtual void Reset() = 0;
        virtual void Execute(Array<GraphicsSemaphore*> waitForSemaphores = {}) = 0;

        virtual void SetResourceState(GraphicsBuffer* buffer, GraphicsResourceState state) = 0;

        virtual void CopyBuffer(GraphicsBuffer* source, GraphicsBuffer* destination) = 0;

        virtual void ClearRenderTarget(GraphicsBuffer* renderTarget, FloatColor color) = 0;
    };
}
