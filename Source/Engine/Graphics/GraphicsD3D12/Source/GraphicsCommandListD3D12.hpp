#pragma once
#include <GraphicsCommandList.hpp>
#include <d3d12.h>

namespace Faro
{
    class GraphicsCommandListD3D12 : public GraphicsCommandList
    {
    public:
        void Init(GraphicsAdapter* adapter) override;
        void Destroy() override;

        void Reset() override;
        void Execute() override;

    private:
        ID3D12CommandAllocator* allocator = nullptr;
        ID3D12GraphicsCommandList* commandList = nullptr;
    };
}
