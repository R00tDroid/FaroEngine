#pragma once
#include <GraphicsPipeline.hpp>

namespace Faro
{
    class GraphicsPipelineD3D12 : public GraphicsPipeline
    {
    public:
        using GraphicsPipeline::Init;

        void Init() override;
        void Destroy() override;
    };
}
