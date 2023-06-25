#pragma once
#include <GraphicsAdapter.hpp>

namespace Faro
{
    class GraphicsAdapterVK : public GraphicsAdapter
    {
    public:
        void Init(GraphicsAdapterDesc&) override;
        void Destroy() override;

        GraphicsCommandList* CreateCommandList() override;

        GraphicsBuffer* CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc) override;

        GraphicsFence* CreateFence() override;

        GraphicsSwapchain* CreateSwapchain(Window* window) override;

        GraphicsPipeline* CreatePipeline(GraphicsPipelineDesc desc) override;
    };
}
