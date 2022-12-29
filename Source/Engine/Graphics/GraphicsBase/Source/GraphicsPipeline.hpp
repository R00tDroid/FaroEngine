#pragma once
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    struct GraphicsPipelineDesc
    {
    };

    class GraphicsPipeline : public IGraphicsAdapterChild
    {
    public:
        using IGraphicsAdapterChild::Init;
        void Init(GraphicsAdapter* adapter, GraphicsPipelineDesc desc);

    protected:
        GraphicsPipelineDesc desc;
    };
}
