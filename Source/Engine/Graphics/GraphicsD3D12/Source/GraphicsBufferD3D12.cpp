#if FARO_OS_WINDOWS
#include "GraphicsBufferD3D12.hpp"
#include <GraphicsAdapterD3D12.hpp>
#include <directx/d3dx12.h>
#include <Memory/MemoryManager.hpp>
#include <GraphicsSwapchainD3D12.hpp>

namespace Faro
{
    D3D12_RESOURCE_STATES Convert(GraphicsResourceState state)
    {
        switch (state)
        {
            case RS_Unknown: return D3D12_RESOURCE_STATE_COMMON;
            case RS_CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
            case RS_CopyDestination: return D3D12_RESOURCE_STATE_COPY_DEST;
            case RS_RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case RS_ShaderResource: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case RS_Present: return D3D12_RESOURCE_STATE_PRESENT;
        }
        return D3D12_RESOURCE_STATE_COMMON;
    }

    void IGraphicsBufferD3D12::Destroy()
    {
        if (descriptorHeap != nullptr) 
        {
            descriptorHeap->Release();
            descriptorHeap = nullptr;
        }

        gpuResource->Release();
        gpuResource = nullptr;

        cpuAddress = nullptr;

        GraphicsBuffer::Destroy();
    }

    void IGraphicsBufferD3D12::TransitionResource(ID3D12GraphicsCommandList* commandList, GraphicsResourceState state)
    {
        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(gpuResource, Convert(GetResourceState()), Convert(state));
        commandList->ResourceBarrier(1, &resourceBarrier);

        SetResourceState(state);
    }

    ID3D12Resource* IGraphicsBufferD3D12::GetResource()
    {
        return gpuResource;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE IGraphicsBufferD3D12::GetDescriptor()
    {
        if (descriptorHeap != nullptr)
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, descriptorHeapSize);
        }
        return {};
    }

    void GraphicsBufferUploadD3D12::Init()
    {
        IGraphicsAdapterChild::Init();

        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetDesc().dataSize);

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&gpuResource));
        gpuResource->SetName(L"GraphicsBufferUploadD3D12");

        CD3DX12_RANGE readRange(0, 0);
        gpuResource->Map(0, &readRange, reinterpret_cast<void**>(&cpuAddress));

        SetResourceState(RS_Unknown);
    }

    void GraphicsBufferUploadD3D12::Upload(uint8* data)
    {
        MemoryManager::Copy<uint8>(data, cpuAddress, GetDesc().dataSize);
    }

    void GraphicsBufferRemoteD3D12::Init()
    {
        IGraphicsAdapterChild::Init();

        if (GetDesc().texture.renderTarget && GetDesc().renderTarget.swapchain != nullptr)
        {
            GraphicsSwapchainD3D12* swapchain = static_cast<GraphicsSwapchainD3D12*>(GetDesc().renderTarget.swapchain);
            swapchain->GetNativeSwapchain()->GetBuffer(GetDesc().renderTarget.swapchainImageIndex, IID_PPV_ARGS(&gpuResource));
            gpuResource->SetName(L"GraphicsBufferRemoteD3D12::SwapchainImage");
            //TODO set dimensions to desc
            SetResourceState(RS_Unknown);

            D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
            descriptorHeapDesc.NumDescriptors = 2;
            descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            ID3D12Device* device = GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice();
            device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
            descriptorHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

            CD3DX12_CPU_DESCRIPTOR_HANDLE descriptorHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
            descriptorHandle.Offset(1, descriptorHeapSize);
            device->CreateRenderTargetView(gpuResource, nullptr, descriptorHandle);
        }
        else 
        {
            D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC resourceDesc = GetNativeDesc();

            GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&gpuResource));
            gpuResource->SetName(L"GraphicsBufferRemoteD3D12");

            SetResourceState(RS_Unknown);
        }
    }

    D3D12_RESOURCE_DESC GraphicsBufferRemoteD3D12::GetNativeDesc()
    {
        const GraphicsBufferDesc& bufferDesc = GetDesc();

        D3D12_RESOURCE_DESC desc;
        MemoryManager::Zero(&desc);

        switch (bufferDesc.resourceType)
        {
            case RT_Texture:
            {
                desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, bufferDesc.texture.resolution.x, bufferDesc.texture.resolution.y);
                if (bufferDesc.texture.renderTarget)
                {
                    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                }
                if (bufferDesc.texture.shaderResource)
                {
                    desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                }
                break;
            }

            case RT_ConstantBuffer:
            {
                desc = CD3DX12_RESOURCE_DESC::Buffer(bufferDesc.dataSize);
                break;
            }
            case RT_VertexBuffer:
            {
                desc = CD3DX12_RESOURCE_DESC::Buffer(bufferDesc.dataSize);
                break;
            }
            case RT_IndexBuffer:
            {
                desc = CD3DX12_RESOURCE_DESC::Buffer(bufferDesc.dataSize);
                break;
            }
        }

        return desc;
    }
}
#endif
