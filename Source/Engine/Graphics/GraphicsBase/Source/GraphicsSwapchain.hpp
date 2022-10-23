#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Window.hpp>

namespace Faro
{
    class GraphicsSwapchain : public IGraphicsAdapterChild
    {
    public:
        virtual void Init(GraphicsAdapter* adapter, Window* window);
    };
}
