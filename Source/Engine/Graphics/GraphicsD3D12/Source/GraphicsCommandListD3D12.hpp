#pragma once
#include <GraphicsCommandList.hpp>
#include <d3d12.h>

namespace Faro
{
    class GraphicsCommandListD3D12 : public GraphicsCommandList
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

    private:
        ID3D12CommandAllocator* allocator = nullptr;
        ID3D12GraphicsCommandList* commandList = nullptr;
    };
}
