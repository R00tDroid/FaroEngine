#include "GraphicsPipeline.hpp"
#include "GraphicsLog.hpp"
#include <Memory/MemoryManager.hpp>

//TODO Get shader info from same header as transpiler
#define ShaderPackageVersion 0x0000

#define ShaderBinaryType_DXIL 0x01
#define ShaderBinaryType_SPRIV 0x02

#define ShaderStageType_Vertex 0x11
#define ShaderStageType_Geometry 0x12
#define ShaderStageType_Pixel 0x13

namespace Faro
{
    bool ProcessShaderStage(DataStream* stream, GraphicsPipelineDesc* desc)
    {
        uint8 stageType = stream->Read<uint8>();
        uint32 stageSize = stream->Read<uint32>();

        if (stageSize > 0) 
        {
            if (desc == nullptr)
            {
                stream->Seek(SSO_Current, stageSize);
            }
            else
            {
                uint8** destination = nullptr;
                if (stageType == ShaderStageType_Vertex)
                {
                    desc->vsSize = stageSize;
                    destination = &desc->vsData;
                }
                else if (stageType == ShaderStageType_Geometry)
                {
                    desc->gsSize = stageSize;
                    destination = &desc->gsData;
                }
                else if (stageType == ShaderStageType_Pixel)
                {
                    desc->psSize = stageSize;
                    destination = &desc->psData;
                }
                else
                {
                    GraphicsLog.Log(LC_Error, "Invalid shader stage: %2x", stageType);
                    return false;
                }

                *destination = MemoryManager::Alloc<uint8>(stageSize);
                stream->Read<uint8>(*destination, stageSize);
            }
        }

        return true;
    }

    bool ProcessShaderBinary(DataStream* stream, GraphicsPipelineDesc* desc)
    {
        for (int i = 0; i < 3; i++) 
        {
            if (!ProcessShaderStage(stream, desc)) return false;
        }
        return true;
    }

    bool GraphicsPipeline::LoadShader(DataStream* stream, GraphicsPipelineDesc& desc)
    {
        uint8 header1 = stream->Read<uint8>();
        uint8 header2 = stream->Read<uint8>();

        if (header1 != 0x46 || header2 != 0x53)
        {
            GraphicsLog.Log(LC_Error, "Invalid shader header");
            return false;
        }

        uint16 packageVersion = stream->Read<uint16>();
        if (packageVersion != ShaderPackageVersion)
        {
            GraphicsLog.Log(LC_Error, "Incompatible shader package. Expected %4x, found %4x", ShaderPackageVersion, packageVersion);
            return false;
        }

        while (true)
        {
            if (stream->Tell() >= stream->Size()) break;

            uint8 binaryType = stream->Read<uint8>();

            ProcessShaderBinary(stream, binaryType == ShaderBinaryType_DXIL ? &desc : nullptr); //TODO select based on GGraphics requirements
        }

        return true;
    }

    void GraphicsPipeline::Init(GraphicsAdapter* adapter, GraphicsPipelineDesc inDesc)
    {
        desc = inDesc;
        IGraphicsAdapterChild::Init(adapter);
    }
}
