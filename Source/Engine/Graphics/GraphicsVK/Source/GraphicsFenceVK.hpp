#pragma once
#include <GraphicsFence.hpp>
#include <glad/vulkan.h>

namespace Faro
{
    class GraphicsFenceVK : public GraphicsFence
    {
    public:
        using IGraphicsAdapterChild::Init;

        void Init() override;
        void Destroy() override;

        void Trigger() override;
        bool HasFinished() override;
        void WaitForFinish() override;

    private:
        VkFence fence = nullptr;
    };
}
