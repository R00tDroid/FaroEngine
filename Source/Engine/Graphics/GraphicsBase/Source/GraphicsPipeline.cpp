#include "GraphicsPipeline.hpp"

namespace Faro
{
    void GraphicsPipeline::Init(GraphicsAdapter* adapter, GraphicsPipelineDesc inDesc)
    {
        desc = inDesc;
        IGraphicsAdapterChild::Init(adapter);
    }
}
