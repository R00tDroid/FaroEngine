#pragma once
#include <GraphicsFence.hpp>
#include <d3d12.h>
#include <Primitives.hpp>

namespace Faro
{
    class GraphicsFenceD3D12 : public GraphicsFence
    {
    public:
        void Init(GraphicsAdapter* adapter) override;
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
