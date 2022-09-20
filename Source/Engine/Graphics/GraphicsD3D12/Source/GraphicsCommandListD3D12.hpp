#pragma once
#include <GraphicsCommandList.hpp>

#include "GraphicsAdapterD3D12.hpp"

namespace Faro
{
    class GraphicsCommandListD3D12 : public GraphicsCommandList
    {
    public:
        GraphicsCommandListD3D12(GraphicsAdapterD3D12*);

        void Reset() override;
        void Execute() override;

    private:
        GraphicsAdapterD3D12* adapter = nullptr;
    };
}
