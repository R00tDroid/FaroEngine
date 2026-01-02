#include "ShaderPacker.hpp"

void ShaderPacker::Pack(std::filesystem::path& file, Shader& shader)
{
    stream.open(file, std::ios::binary);
    WriteHeader();
    WriteShader(shader);
    stream.close();
}

void ShaderPacker::WriteHeader()
{
    stream << char(0x46) << char(0x53);
    unsigned short version = ShaderPackageVersion;
    stream.write(reinterpret_cast<char*>(&version), sizeof(unsigned short));
}

void ShaderPacker::WriteShader(Shader& shader)
{
    WriteShaderBinary(shader.dxil, ShaderBinaryType_DXIL);
    WriteShaderBinary(shader.spirV, ShaderBinaryType_SPRIV);
}

void ShaderPacker::WriteShaderBinary(ShaderBinary& binary, unsigned char binaryType)
{
    stream << char(binaryType);
    WriteShaderStage(binary.vertex, ShaderStageType_Vertex);
    WriteShaderStage(binary.geometry, ShaderStageType_Geometry);
    WriteShaderStage(binary.pixel, ShaderStageType_Pixel);
}

void ShaderPacker::WriteShaderStage(ShaderStage& stage, unsigned char stageType)
{
    stream << char(stageType);
    unsigned int size = stage.dataSize;
    stream.write(reinterpret_cast<char*>(&size), sizeof(unsigned int));

    if (size > 0) 
    {
        stream.write(reinterpret_cast<char*>(stage.data), size);
    }
}
