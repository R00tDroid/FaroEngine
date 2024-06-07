#pragma once
#include <GraphicsSemaphore.hpp>
#include "Vulkan.hpp"

namespace Faro
{
    class GraphicsSemaphoreVK : public GraphicsSemaphore
    {
    public:
        using GraphicsSemaphore::Init;

        VkSemaphore GetHandle();

        void Destroy() override;

    protected:
        void Init() override;

    private:
        VkSemaphore semaphore = nullptr;
    };
}
