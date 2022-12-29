#pragma once

struct ShaderStage
{
    unsigned char* data = nullptr;
    unsigned int dataSize = 0;
};

struct ShaderBinary
{
    ShaderStage vertex;
    ShaderStage pixel;
};

struct Shader
{
    ShaderBinary dxil;
    ShaderBinary spirV;
};

#define ShaderBinaryType_DXIL 0x01
#define ShaderBinaryType_SPRIV 0x02

#define ShaderStageType_Vertex 0x01
#define ShaderStageType_Geometry 0x02
#define ShaderStageType_Pixel 0x03

/* --Packaging layout--
 *
 * Header
 *   File identifier, 2x uint8: 0x46 0x53
 *   File version, uint16
 *
 * Shader
 *   ShaderBinaryType, uint8
 *
 *   Shader stage
 *     ShaderStageType, uint8
 *     ShaderStageSize, uint32
 *     ShaderBinaryData, ShaderStageSize-x uint8
 */
