#include "GraphicsRenderTarget.hpp"

namespace Faro
{
    void GraphicsRenderTarget::Destroy()
    {
        ReleaseResources();
        IGraphicsAdapterChild::Destroy();
    }

    void GraphicsRenderTarget::SetClearColor(UInt8Color color)
    {
        clearColor = color;
    }
}
