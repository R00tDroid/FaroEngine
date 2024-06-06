#pragma once
#if FARO_OS_WINDOWS
#include <GraphicsPipeline.hpp>
#include <d3d12.h>

namespace Faro
{
    class GraphicsPipelineD3D12 : public GraphicsPipeline
    {
    public:
        using GraphicsPipeline::Init;

        void Init() override;
        void Destroy() override;

    private:
        void CreateRootSignature();
        void CreatePipelineState();

        ID3D12PipelineState* pipelineState = nullptr;
        ID3D12RootSignature* rootSignature = nullptr;
    };
}
#endif
