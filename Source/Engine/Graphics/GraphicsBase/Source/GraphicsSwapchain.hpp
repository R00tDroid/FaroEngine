#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Window.hpp>
#include <GraphicsBufferContainer.hpp>

namespace Faro
{
    class GraphicsSwapchainImageContainer : public GraphicsBufferContainer
    {
    public:
        void Init(GraphicsAdapter* adapter, GraphicsBufferType type, GraphicsBufferDesc desc) override;
    };

    class GraphicsSwapchain : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, Window* window);
        void Destroy() override;

        virtual void Present() = 0;

        GraphicsSwapchainImageContainer* backbuffer = nullptr;

    protected:
        void CreateBackbuffer();
    };
}
