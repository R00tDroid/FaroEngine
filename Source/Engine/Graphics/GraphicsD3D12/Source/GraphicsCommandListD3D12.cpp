#include "GraphicsCommandListD3D12.hpp"

#include <directx/d3dx12.h>

#include "GraphicsAdapterD3D12.hpp"
#include "GraphicsBufferD3D12.hpp"

namespace Faro
{
    void GraphicsCommandListD3D12::Init(GraphicsAdapter* adapter)
    {
        GraphicsCommandList::Init(adapter);

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

    void GraphicsCommandListD3D12::Execute()
    {
        commandList->Close();

        ID3D12CommandList* CommandLists[] = { commandList };
        GetTypedAdapter<GraphicsAdapterD3D12>()->GetCommandQueue()->ExecuteCommandLists(_countof(CommandLists), CommandLists);
    }

    void GraphicsCommandListD3D12::SetResourceState(GraphicsBuffer* bufferPtr, GraphicsResourceState state)
    {
        IGraphicsBufferD3D12* buffer = static_cast<IGraphicsBufferD3D12*>(bufferPtr);
        buffer->TransitionResource(commandList, state);
    }
}
