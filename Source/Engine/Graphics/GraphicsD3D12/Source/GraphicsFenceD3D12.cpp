#include "GraphicsFenceD3D12.hpp"
#include <GraphicsAdapterD3D12.hpp>

namespace Faro
{
    void GraphicsFenceD3D12::Init()
    {
        GraphicsFence::Init();

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateFence(triggerValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

        fenceEvent = CreateEventA(nullptr, false, false, nullptr);
    }

    void GraphicsFenceD3D12::Destroy()
    {
        fence->Release();
        fence = nullptr;

        CloseHandle(fenceEvent);
        fenceEvent = nullptr;

        GraphicsFence::Destroy();
    }

    void GraphicsFenceD3D12::Trigger()
    {
        triggerValue++;
        GetTypedAdapter<GraphicsAdapterD3D12>()->GetCommandQueue()->Signal(fence, triggerValue);
    }

    bool GraphicsFenceD3D12::HasFinished()
    {
        return fence->GetCompletedValue() == triggerValue;
    }

    void GraphicsFenceD3D12::WaitForFinish()
    {
        if (HasFinished()) return;

        fence->SetEventOnCompletion(triggerValue, fenceEvent);
        WaitForSingleObject(fenceEvent, 64);
    }
}
