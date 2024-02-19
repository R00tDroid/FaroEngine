#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Window.hpp>
#include <GraphicsBufferContainer.hpp>

#include "GraphicsSyncPoint.hpp"

namespace Faro
{
    class GraphicsSwapchainImageContainer : public GraphicsBufferContainer
    {
    public:
        void Init(GraphicsAdapter* adapter, GraphicsBufferCreateDesc createDesc) override;
    };

    class GraphicsSwapchain : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, Window* window);
        void Destroy() override;

        virtual void Present() = 0;

        GraphicsSwapchainImageContainer* backbuffer = nullptr;

        GraphicsSyncPoint* GetSyncPoint(uint8 imageIndex);

    protected:
        void CreateBackbuffer();
        Array<GraphicsSyncPoint*> bufferReadySync;
    };
}
