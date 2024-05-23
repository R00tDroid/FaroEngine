#pragma once
#if FARO_OS_WINDOWS
#include <GraphicsFence.hpp>
#include <d3d12.h>
#include <Primitives.hpp>

namespace Faro
{
    class GraphicsFenceD3D12 : public GraphicsFence
    {
    public:
        using IGraphicsAdapterChild::Init;

        void Init() override;
        void Destroy() override;

        void Trigger() override;
        bool HasFinished() override;
        void WaitForFinish() override;

    private:
        uint32 triggerValue = 0;

        ID3D12Fence* fence = nullptr;
        HANDLE fenceEvent = nullptr;
    };
}
#endif
