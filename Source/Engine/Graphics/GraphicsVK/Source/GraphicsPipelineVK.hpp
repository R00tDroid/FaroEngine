#pragma once
#include <GraphicsPipeline.hpp>

namespace Faro
{
    class GraphicsPipelineVK : public GraphicsPipeline
    {
    public:
        using GraphicsPipeline::Init;

        void Init() override;
        void Destroy() override;
    };
}
