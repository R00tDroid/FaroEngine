#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Primitives.hpp>
#include <Containers/Stream.hpp>

namespace Faro
{
    struct GraphicsPipelineDesc
    {
        uint8* vsData = nullptr;
        uint16 vsSize = 0;

        uint8* gsData = nullptr;
        uint16 gsSize = 0;

        uint8* psData = nullptr;
        uint16 psSize = 0;
    };

    class GraphicsPipeline : public IGraphicsAdapterChild
    {
    public:
        static bool LoadShader(DataStream* stream, GraphicsPipelineDesc& desc);

        using IGraphicsAdapterChild::Init;
        void Init(GraphicsAdapter* adapter, GraphicsPipelineDesc desc);

    protected:
        GraphicsPipelineDesc desc;
    };
}
