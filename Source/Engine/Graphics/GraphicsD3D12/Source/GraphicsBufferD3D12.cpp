#include "GraphicsBufferD3D12.hpp"
#include "GraphicsAdapterD3D12.hpp"
#include <directx/d3dx12.h>
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    void IGraphicsBufferD3D12::Destroy()
    {
        gpuResource->Release();
        gpuResource = nullptr;

        cpuAddress = nullptr;

        GraphicsBuffer::Destroy();
    }

    void GraphicsBufferUploadD3D12::Init()
    {
        IGraphicsAdapterChild::Init();

        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetDesc().dataSize);

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&gpuResource));
        gpuResource->SetName(L"GraphicsBufferUploadD3D12");
    }

    void GraphicsBufferUploadD3D12::Upload(uint8* data)
    {
        MemoryManager::Copy<uint8>(data, cpuAddress, GetDesc().dataSize);
    }

    void GraphicsBufferRemoteD3D12::Init()
    {
        IGraphicsAdapterChild::Init();

        D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC resourceDesc = GetNativeDesc();

        GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&gpuResource));
        gpuResource->SetName(L"GraphicsBufferRemoteD3D12");
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
