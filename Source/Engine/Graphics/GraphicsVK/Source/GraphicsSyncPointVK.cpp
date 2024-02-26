#include "GraphicsSyncPointVK.hpp"

#include "Assert.hpp"
#include "GraphicsAdapterVK.hpp"

namespace Faro
{
    VkSemaphore GraphicsSyncPointVK::GetHandle()
    {
        return semaphore;
    }

    void GraphicsSyncPointVK::Destroy()
    {
        if (semaphore != nullptr)
        {
            GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();
            vkDestroySemaphore(adapter->GetDevice(), semaphore, nullptr);
            semaphore = nullptr;
        }

        GraphicsSyncPoint::Destroy();
    }

    void GraphicsSyncPointVK::Init()
    {
        GraphicsSyncPoint::Init();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();

        VkSemaphoreCreateInfo desc {};
        desc.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(adapter->GetDevice(), &desc, nullptr, &semaphore);

        Debug_Assert(semaphore != nullptr);
    }
}
