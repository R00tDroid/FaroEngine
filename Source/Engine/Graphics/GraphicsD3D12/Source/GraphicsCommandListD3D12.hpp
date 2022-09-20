#pragma once
#include <GraphicsCommandList.hpp>

namespace Faro
{
    class GraphicsCommandListD3D12 : public GraphicsCommandList
    {
    public:
        void Reset() override;
        void Execute() override;
    };
}
