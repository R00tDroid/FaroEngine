#pragma once
#include <GraphicsPipeline.hpp>

namespace Faro
{
    class GraphicsPipelineD3D12 : public GraphicsPipeline
    {
    public:
        void Init() override;
        void Destroy() override;
    };
}
