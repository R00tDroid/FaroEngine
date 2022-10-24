#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Math/Vector2.hpp>
#include <Math/Color.hpp>
#include <GraphicsSwapchain.hpp>

namespace Faro
{
    class GraphicsRenderTarget : public IGraphicsAdapterChild
    {
    public:

        virtual void CreateFromSwapchain(GraphicsSwapchain* swapchain) = 0;
        virtual void Create(Int2D resolution) = 0;
        virtual void ReleaseResources() = 0;

        void SetClearColor(UInt8Color color);

    private:
        UInt8Color clearColor = UInt8Color::Clear;
    };
}
