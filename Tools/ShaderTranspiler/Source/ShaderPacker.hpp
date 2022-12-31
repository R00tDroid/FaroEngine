#pragma once
#include <Utility.hpp>
#include <ShaderDefinition.hpp>

class ShaderPacker
{
public:
    void Pack(std::filesystem::path& file, Shader& shader);

private:
    void WriteHeader();
    void WriteShader(Shader& shader);
    void WriteShaderBinary(ShaderBinary& binary, unsigned char binaryType);
    void WriteShaderStage(ShaderStage& stage, unsigned char stageType);

    std::ofstream stream;
};
