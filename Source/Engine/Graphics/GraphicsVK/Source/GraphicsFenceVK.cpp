#include "GraphicsFenceVK.hpp"
#include <GraphicsAdapterVK.hpp>
#include <Assert.hpp>

namespace Faro
{
    void GraphicsFenceVK::Init()
    {
        GraphicsFence::Init();

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        vkCreateFence(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), &fenceInfo, nullptr, &fence);

        Debug_Assert(fence != nullptr);
    }

    void GraphicsFenceVK::Destroy()
    {
        if (fence != nullptr) 
        {
            vkDestroyFence(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), fence, nullptr);
            fence = nullptr;
        }
        GraphicsFence::Destroy();
    }

    void GraphicsFenceVK::Trigger()
    {
        vkResetFences(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), 1, &fence);
        vkQueueSubmit(GetTypedAdapter<GraphicsAdapterVK>()->GetQueue(), 0, nullptr, fence);
    }

    bool GraphicsFenceVK::HasFinished()
    {
        return vkGetFenceStatus(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), fence) == VK_SUCCESS;
    }

    void GraphicsFenceVK::WaitForFinish()
    {
        if (HasFinished()) return;
        vkWaitForFences(GetTypedAdapter<GraphicsAdapterVK>()->GetDevice(), 1, &fence, true, 1e5);
    }
}
