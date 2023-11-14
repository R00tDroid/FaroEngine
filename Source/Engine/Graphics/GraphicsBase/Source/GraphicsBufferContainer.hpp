#pragma once
#include <GraphicsBuffer.hpp>

namespace Faro
{
    class GraphicsBufferContainer : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, GraphicsBufferCreateDesc createDesc);
        void Destroy() override;

        GraphicsBuffer* Get();

        GraphicsBufferCreateDesc createDesc;

    protected:
        GraphicsBuffer* resources[2] = { nullptr, nullptr };
    };
}
