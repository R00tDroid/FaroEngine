#include "GraphicsPipelineD3D12.hpp"
#include <directx/d3dx12.h>
#include <GraphicsLogD3D12.hpp>
#include <Containers/String.hpp>
#include <GraphicsAdapterD3D12.hpp>
#include <ConversionD3D12.hpp>

namespace Faro
{
    void GraphicsPipelineD3D12::Init()
    {
        GraphicsPipeline::Init();

        CreateRootSignature();
        CreatePipelineState();
    }

    void GraphicsPipelineD3D12::Destroy()
    {
        rootSignature->Release();
        pipelineState->Release();

        GraphicsPipeline::Destroy();
    }

    void GraphicsPipelineD3D12::CreateRootSignature()
    {
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob* signatureBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (errorBlob != nullptr)
        {
            String message(static_cast<char*>(errorBlob->GetBufferPointer()), uint32(errorBlob->GetBufferSize()));
            GraphicsLogD3D12.Log(LC_Error, "Failed to serialize root signature: %s", message.Data());
            errorBlob->Release();
        }

        if (FAILED(GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
        {
            GraphicsLogD3D12.Log(LC_Error, "Failed to create root signature");
        }

        signatureBlob->Release();
    }

    void GraphicsPipelineD3D12::CreatePipelineState()
    {
        D3D12_INPUT_ELEMENT_DESC* inputLayout = nullptr;

        if (!desc.inputLayout.Empty()) 
        {
            inputLayout = MemoryManager::Alloc<D3D12_INPUT_ELEMENT_DESC>(desc.inputLayout.Size());
            MemoryManager::Zero<D3D12_INPUT_ELEMENT_DESC>(inputLayout, desc.inputLayout.Size());

            uint32 padding = 0;
            for (uint32 i = 0; i < desc.inputLayout.Size(); i++)
            {
                inputLayout[i].SemanticName = desc.inputLayout[i].name.Data();
                inputLayout[i].SemanticIndex = desc.inputLayout[i].semanticIndex;
                inputLayout[i].Format = Convert(desc.inputLayout[i].format);
                inputLayout[i].AlignedByteOffset = padding;

                padding += desc.inputLayout[i].format.GetStride();
            }
        }

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
        MemoryManager::Zero(&pipelineDesc);
        pipelineDesc.InputLayout = { inputLayout, desc.inputLayout.Size() };
        pipelineDesc.pRootSignature = rootSignature;
        pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipelineDesc.SampleMask = UINT_MAX;
        pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pipelineDesc.SampleDesc.Count = 1;
        pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        pipelineDesc.NumRenderTargets = desc.renderTargets.Size();
        for (uint32 i = 0; i < desc.renderTargets.Size(); i++) 
        {
            pipelineDesc.RTVFormats[i] = Convert(desc.renderTargets[0]);
        }

        pipelineDesc.VS = D3D12_SHADER_BYTECODE{ desc.vsData, desc.vsSize };
        pipelineDesc.GS = D3D12_SHADER_BYTECODE{ desc.gsData, desc.gsSize };
        pipelineDesc.PS = D3D12_SHADER_BYTECODE{ desc.psData, desc.psSize };

        if (FAILED(GetTypedAdapter<GraphicsAdapterD3D12>()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState))))
        {
            GraphicsLogD3D12.Log(LC_Error, "Failed to create pipeline state");
        }

        MemoryManager::Free(inputLayout);
    }
}
