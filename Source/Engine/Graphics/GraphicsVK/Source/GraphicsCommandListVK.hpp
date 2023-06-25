#pragma once
#include <GraphicsCommandList.hpp>

namespace Faro
{
    class GraphicsCommandListVK : public GraphicsCommandList
    {
    public:
        using IGraphicsAdapterChild::Init;

        void Init() override;

        void Destroy() override;

        void Reset() override;
        void Execute() override;

        void SetResourceState(GraphicsBuffer* buffer, GraphicsResourceState state) override;

        void CopyBuffer(GraphicsBuffer* source, GraphicsBuffer* destination) override;

        void ClearRenderTarget(GraphicsBuffer* renderTarget, FloatColor color) override;
    };
}
