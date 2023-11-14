#pragma once
#if FARO_OS_WINDOWS
#include <GraphicsAdapter.hpp>
#include <d3d12.h>

namespace Faro
{
    class GraphicsBufferD3D12 : public GraphicsBuffer
    {
    public:
        void Init() override;

        void Destroy() override;

        void TransitionResource(ID3D12GraphicsCommandList* commandList, GraphicsResourceState state);

        void Upload(uint8* data) override;

        ID3D12Resource* GetResource();
        D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptor();

    private:
        D3D12_RESOURCE_DESC GetNativeDesc();

        uint8* cpuAddress = nullptr;
        ID3D12Resource* gpuResource = nullptr;

        ID3D12DescriptorHeap* descriptorHeap = nullptr;
        uint32 descriptorHeapSize = 0;
    };
}
#endif
