#include "GraphicsPipelineD3D12.hpp"
#include <directx/d3dx12.h>
#include <GraphicsLogD3D12.hpp>
#include <Containers/String.hpp>
#include <GraphicsAdapterD3D12.hpp>

namespace Faro
{
    void GraphicsPipelineD3D12::Init()
    {
        GraphicsPipeline::Init();

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob* signatureBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (errorBlob != nullptr)
        {
            String message(static_cast<char*>(errorBlob->GetBufferPointer()), uint32(errorBlob->GetBufferSize()));
            GraphicsLogD3D12.Log(LC_Error, "Failed to serialize root signature: %s", message.Data());
        }

        if (FAILED(GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateGraphicsPipelineState(nullptr, IID_PPV_ARGS(&pipelineState))))
        {
            GraphicsLogD3D12.Log(LC_Error, "Failed to create pipeline state");
        }
    }

    void GraphicsPipelineD3D12::Destroy()
    {
        rootSignature->Release();
        pipelineState->Release();

        GraphicsPipeline::Destroy();
    }
}
