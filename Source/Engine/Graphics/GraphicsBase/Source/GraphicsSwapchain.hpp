#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Window.hpp>
#include <GraphicsBufferContainer.hpp>

namespace Faro
{
    class GraphicsSwapchain : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, Window* window);
        void Destroy() override;

        GraphicsBufferContainer* backbuffer = nullptr;
    };
}
