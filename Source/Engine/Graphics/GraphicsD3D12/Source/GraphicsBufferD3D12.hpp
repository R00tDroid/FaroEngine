#pragma once
#include <GraphicsAdapter.hpp>
#include <d3d12.h>

namespace Faro
{
    class IGraphicsBufferD3D12 : public GraphicsBuffer
    {
    public:
        void Destroy() override;

    protected:
        uint8* cpuAddress = nullptr;
        ID3D12Resource* gpuResource = nullptr;
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
