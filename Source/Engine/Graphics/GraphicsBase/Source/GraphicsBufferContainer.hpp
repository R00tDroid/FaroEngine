#pragma once
#include <GraphicsBuffer.hpp>

namespace Faro
{
    class GraphicsBufferContainer : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, GraphicsBufferType type, GraphicsBufferDesc desc);
        virtual void Destroy() override;

        GraphicsBuffer* Get();

        GraphicsBufferType type;
        GraphicsBufferDesc desc;

    protected:
        GraphicsBuffer* resources[2] = { nullptr, nullptr };
    };
}
