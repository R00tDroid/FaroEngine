#if FARO_OS_WINDOWS
#include "GraphicsCommandListD3D12.hpp"
#include <directx/d3dx12.h>
#include "GraphicsAdapterD3D12.hpp"
#include "GraphicsBufferD3D12.hpp"

namespace Faro
{
    void GraphicsCommandListD3D12::Init()
    {
        GraphicsCommandList::Init();

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, nullptr, IID_PPV_ARGS(&commandList));

        commandList->Close();
    }

    void GraphicsCommandListD3D12::Destroy()
    {
        allocator->Release();

        commandList->Release();

        GraphicsCommandList::Destroy();
    }

    void GraphicsCommandListD3D12::Reset()
    {
        allocator->Reset();
        commandList->Reset(allocator, nullptr);
    }

    void GraphicsCommandListD3D12::Execute(Array<GraphicsSemaphore*> waitForSemaphores)
    {
        commandList->Close();

        ID3D12CommandList* CommandLists[] = { commandList };
        GetTypedAdapter<GraphicsAdapterD3D12>()->GetCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);
    }

    void GraphicsCommandListD3D12::SetResourceState(GraphicsBuffer* bufferPtr, GraphicsResourceState state)
    {
        if (bufferPtr->GetResourceState() == state) return;

        GraphicsBufferD3D12* buffer = static_cast<GraphicsBufferD3D12*>(bufferPtr);
        buffer->TransitionResource(commandList, state);
    }

    void GraphicsCommandListD3D12::CopyBuffer(GraphicsBuffer* sourcePtr, GraphicsBuffer* destinationPtr)
    {
        GraphicsBufferD3D12* source = static_cast<GraphicsBufferD3D12*>(sourcePtr);
        GraphicsBufferD3D12* destination = static_cast<GraphicsBufferD3D12*>(destinationPtr);

        commandList->CopyResource(destination->GetResource(), source->GetResource());
    }

    void GraphicsCommandListD3D12::ClearRenderTarget(GraphicsBuffer* inRenderTarget, FloatColor color)
    {
        GraphicsBufferD3D12* renderTarget = static_cast<GraphicsBufferD3D12*>(inRenderTarget);
        commandList->ClearRenderTargetView(renderTarget->GetDescriptor(), color.d, 0, nullptr);
    }
}
#endif
