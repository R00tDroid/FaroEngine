#include "GraphicsSwapchain.hpp"
#include <Memory/MemoryManager.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    void GraphicsSwapchainImageContainer::Init(GraphicsAdapter* adapter, GraphicsBufferCreateDesc inCreateDesc)
    {
        IGraphicsAdapterChild::Init(adapter);
        createDesc = inCreateDesc;

        GraphicsBufferDesc desc1 = createDesc.bufferDesc;
        desc1.renderTarget.swapchainImageIndex = 0;
        resources[0] = GetAdapter()->CreateBuffer({ createDesc.bufferType, desc1 });

        GraphicsBufferDesc desc2 = createDesc.bufferDesc;
        desc2.renderTarget.swapchainImageIndex = 1;
        resources[1] = GetAdapter()->CreateBuffer({ createDesc.bufferType, desc2 });
    }

    void GraphicsSwapchain::Init(GraphicsAdapter* adapter, Window* window)
    {
        IGraphicsAdapterChild::Init(adapter);
    }

    void GraphicsSwapchain::Destroy()
    {
        MemoryManager::SafeDelete(backbuffer);
        IGraphicsAdapterChild::Destroy();
    }

    void GraphicsSwapchain::CreateBackbuffer()
    {
        GraphicsBufferDesc backbufferDesc = GraphicsBufferDesc::SwapchainImage(this, 0);
        backbuffer = GetAdapter()->CreateBufferContainer<GraphicsSwapchainImageContainer>({ BT_Remote, backbufferDesc });
    }
}
