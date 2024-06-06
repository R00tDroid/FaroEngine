#pragma once
#if FARO_OS_WINDOWS
#include <GraphicsAdapter.hpp>
#include <d3d12.h>

namespace Faro
{
    class IGraphicsBufferD3D12 : public GraphicsBuffer
    {
    public:
        void Destroy() override;

        void TransitionResource(ID3D12GraphicsCommandList* commandList, GraphicsResourceState state);

        ID3D12Resource* GetResource();
        D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptor();

    protected:
        uint8* cpuAddress = nullptr;
        ID3D12Resource* gpuResource = nullptr;

        ID3D12DescriptorHeap* descriptorHeap = nullptr;
        uint32 descriptorHeapSize = 0;
    };

    class GraphicsBufferUploadD3D12 : public IGraphicsBufferD3D12
    {
    public:
        void Init() override;

        void Upload(uint8* data) override;
    };

    class GraphicsBufferRemoteD3D12 : public IGraphicsBufferD3D12
    {
    public:
        void Init() override;

        void Upload(uint8*) override {}

    private:
        D3D12_RESOURCE_DESC GetNativeDesc();
    };
}
#endif
