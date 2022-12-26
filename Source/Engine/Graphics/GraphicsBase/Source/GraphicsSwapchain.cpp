#include "GraphicsSwapchain.hpp"
#include <Memory/MemoryManager.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    void GraphicsSwapchain::Init(GraphicsAdapter* adapter, Window* window)
    {
        IGraphicsAdapterChild::Init(adapter);

        GraphicsBufferDesc backbufferDesc = GraphicsBufferDesc::SwapchainImage(this, 0); //TODO get both images
        GetAdapter()->CreateBufferContainer(BT_Remote, backbufferDesc);
    }

    void GraphicsSwapchain::Destroy()
    {
        MemoryManager::SafeDelete(backbuffer);
        IGraphicsAdapterChild::Destroy();
    }
}
