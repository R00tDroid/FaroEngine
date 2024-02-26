#pragma once
#include <GraphicsSyncPoint.hpp>
#include "Vulkan.hpp"

namespace Faro
{
    class GraphicsSyncPointVK : public GraphicsSyncPoint
    {
    public:
        using GraphicsSyncPoint::Init;

        VkSemaphore GetHandle();

        void Destroy() override;

    protected:
        void Init() override;

    private:
        VkSemaphore semaphore = nullptr;
    };
}
