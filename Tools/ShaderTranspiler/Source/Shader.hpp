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
