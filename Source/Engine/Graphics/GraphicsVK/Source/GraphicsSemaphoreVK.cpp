#include "GraphicsSemaphoreVK.hpp"

#include "Assert.hpp"
#include "GraphicsAdapterVK.hpp"

namespace Faro
{
    VkSemaphore GraphicsSemaphoreVK::GetHandle()
    {
        return semaphore;
    }

    void GraphicsSemaphoreVK::Destroy()
    {
        if (semaphore != nullptr)
        {
            GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();
            vkDestroySemaphore(adapter->GetDevice(), semaphore, nullptr);
            semaphore = nullptr;
        }

        GraphicsSemaphore::Destroy();
    }

    void GraphicsSemaphoreVK::Init()
    {
        GraphicsSemaphore::Init();

        GraphicsAdapterVK* adapter = GetTypedAdapter<GraphicsAdapterVK>();

        VkSemaphoreCreateInfo desc {};
        desc.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        vkCreateSemaphore(adapter->GetDevice(), &desc, nullptr, &semaphore);

        Debug_Assert(semaphore != nullptr);
    }
}
