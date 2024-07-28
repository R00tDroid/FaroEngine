#include "ShaderCompiler.hpp"
#include <Command.hpp>
#include <fstream>

ShaderCompiler::ShaderCompiler()
{
#ifdef _WIN32
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    dxcUtils->CreateDefaultIncludeHandler(&defaultHandler);
#endif
}

ShaderCompiler::~ShaderCompiler()
{
#ifdef _WIN32
    defaultHandler->Release();
    defaultHandler = nullptr;

    dxcUtils->Release();
    dxcUtils = nullptr;
#endif
}

#ifdef _WIN32
HRESULT ShaderCompiler::QueryInterface(const IID& riid, void** ppvObject)
{
    return defaultHandler->QueryInterface(riid, ppvObject);
}

HRESULT ShaderCompiler::LoadSource(LPCWSTR pFilename, IDxcBlob**)// ppIncludeSource)
{
    std::string fileName = Utility::Convert(std::wstring(pFilename));

    /*for(std::filesystem::path& directory : includeDirectories)
    {
        std::filesystem::path filePath = directory / fileName;
        if (std::filesystem::exists(filePath))
        {
            IDxcBlobEncoding* blob = LoadFileToBlob(filePath);
            *ppIncludeSource = blob;

            return S_OK;
        }
    }*/

    return S_FALSE;
}

IDxcBlobEncoding* ShaderCompiler::LoadFileToBlob(std::filesystem::path& filePath)
{
    std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);
    std::streamsize size = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::vector<char> data(size);
    fileStream.read(data.data(), size);
    fileStream.close();

    IDxcBlobEncoding* blob;
    dxcUtils->CreateBlob(data.data(), (UINT32)data.size(), CP_UTF8, &blob);
    return blob;
}
#endif

void ShaderCompiler::SetSearchDirectory(std::vector<std::filesystem::path>&) //directories)
{
    //searchDirectories = directories;
}

bool ShaderCompiler::CompileShader(std::filesystem::path& file, ShaderStage& output, CompilerFlavor compilerFlavor, ShaderType shaderType, bool debug)
{
    output.data = nullptr;
    output.dataSize = 0;

#ifdef _WIN32
    std::vector<LPWSTR> additionalArguments;
    additionalArguments.push_back(L"-Qstrip_debug");
    additionalArguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
    additionalArguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

    if (compilerFlavor == CF_DXIL)
    {
        additionalArguments.push_back(L"-Qstrip_reflect");
    }
    else if (compilerFlavor == CF_SpirV)
    {
        additionalArguments.push_back(L"-spirv");
    }

    if (debug)
    {
        additionalArguments.push_back(DXC_ARG_DEBUG);
    }

    IDxcCompiler3* dxCompiler = nullptr;
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxCompiler));

    std::wstring entrypoint, profile;
    if (shaderType == SS_Vertex)
    {
        entrypoint = L"VSMain";
        profile = L"vs_6_2";
    }
    else if (shaderType == SS_Pixel)
    {
        entrypoint = L"PSMain";
        profile = L"ps_6_2";
    }

    IDxcCompilerArgs* dxArgs = nullptr;
    dxcUtils->BuildArguments(Utility::Convert(file.string()).c_str(), entrypoint.c_str(), profile.c_str(), const_cast<LPCWSTR*>(additionalArguments.data()), static_cast<UINT32>(additionalArguments.size()), nullptr, 0, &dxArgs);

    //includeDirectories = searchDirectories;
    //includeDirectories.push_back(file.parent_path());

    IDxcBlobEncoding* sourceBlob = LoadFileToBlob(file);
    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();
    sourceBuffer.Encoding = 0;

    IDxcResult* compilerResult;
    if (FAILED(dxCompiler->Compile(&sourceBuffer, dxArgs->GetArguments(), dxArgs->GetCount(), this, IID_PPV_ARGS(&compilerResult))))
    {
        Utility::PrintLineD("Compilation failed");

        sourceBlob->Release();
        return false;
    }

    sourceBlob->Release();

    IDxcBlobUtf8* errorBuffer = nullptr;
    if (SUCCEEDED(compilerResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errorBuffer), nullptr)))
    {
        if (errorBuffer != nullptr && errorBuffer->GetStringLength() > 0)
        {
            Utility::Print(static_cast<char*>(errorBuffer->GetBufferPointer()));
        }
    }

    IDxcBlob* binaryBlob = nullptr;
    if (SUCCEEDED(compilerResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&binaryBlob), nullptr)) && binaryBlob != nullptr)
    {
        output.dataSize = (unsigned int)binaryBlob->GetBufferSize();
        output.data = static_cast<unsigned char*>(malloc(output.dataSize));
        std::memcpy(output.data, binaryBlob->GetBufferPointer(), output.dataSize);

        return true;
    }
    else
    {
        Utility::PrintLineD("Failed to get compilation output");
        return false;
    }
#else
    Utility::PrintLine("Compilation not support for: " + file.string() + " (" + std::to_string(compilerFlavor) + " " + std::to_string(shaderType) + " " + std::to_string(debug) + ")");
    return false;
#endif
}


