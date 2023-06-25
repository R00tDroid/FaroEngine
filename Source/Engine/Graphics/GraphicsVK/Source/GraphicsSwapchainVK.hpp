#pragma once
#include <GraphicsSwapchain.hpp>

namespace Faro
{
    class GraphicsSwapchainVK : public GraphicsSwapchain
    {
    public:
        void Init(GraphicsAdapter* adapter, Window* window) override;
        void Destroy() override;

        void Present() override;
    };
}
