#include <Utility.hpp>
#include <Parameters.hpp>
#include "Command.hpp"
#include "Shader.h"
#include "Version.generated.hpp"
#include "VulkanSDKInfo.hpp"

#ifdef _WIN32
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#endif

void PrintHelp()
{
    Utility::PrintLine("Faro Engine shader transpiler " + std::string(EngineVersion));
    Utility::Print("-shader <shader paths> Path to one or more shader file to compile\n");
    Utility::Print("-include <directory paths> Path to one or more directories for include scanning\n");
}

std::filesystem::path dxcExe;
std::vector<std::filesystem::path> shaderFiles;

bool ParseParameters(ParameterList& parameters)
{
    if (parameters.Contains("shader"))
    {
        if (parameters.CountArguments("shader") == 0)
        {
            Utility::PrintLine("-shader requires at least one path");
            return false;
        }

        bool anyFailed = false;
        for (std::string shaderPathString : parameters.GetArguments("shader"))
        {
            std::filesystem::path shaderPath = std::filesystem::absolute(shaderPathString);
            shaderPath = weakly_canonical(shaderPath);
            shaderPath.make_preferred();

            if (!std::filesystem::exists(shaderPath))
            {
                Utility::PrintLine("Could not find input file: " + shaderPath.string());
                anyFailed = true;
                continue;
            }

            shaderFiles.push_back(shaderPath);
        }

        if (anyFailed) return false;
    }
    return true;
}

bool LoadEnvironment()
{
    const std::vector<VulkanSDK>& vulkanSDKs = GetVulkanSDKs();
    for (const VulkanSDK& vulkanSDK : vulkanSDKs)
    {
        std::filesystem::path path = std::filesystem::path(vulkanSDK.Root) / "bin" / "dxc.exe";
        if (std::filesystem::exists(path))
        {
            dxcExe = path;
            break;
        }
    }

    if (dxcExe.empty())
    {
        Utility::PrintLine("Unable to find dxc.exe");
        return false;
    }

    return true;
}

class IncludeHandler : public IDxcIncludeHandler
{
public:
    IncludeHandler(IDxcUtils* dxcUtils) : dxcUtils(dxcUtils)
    {
        dxcUtils->CreateDefaultIncludeHandler(&defaultHandler);
    }
    ~IncludeHandler()
    {
        defaultHandler->Release();
        defaultHandler = nullptr;
    }

    HRESULT QueryInterface(const IID& riid, void** ppvObject) override
    {
        return defaultHandler->QueryInterface(riid, ppvObject);
    }
    ULONG AddRef() override { return 0; }
    ULONG Release() override { return 0; }
    HRESULT LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource) override
    {
        std::string fileName = Utility::Convert(std::wstring(pFilename));

        for(std::filesystem::path& directory : includeDirectories)
        {
            std::filesystem::path filePath = directory / fileName;
            if (std::filesystem::exists(filePath))
            {
                std::ifstream includeFile(filePath, std::ios::binary | std::ios::ate);
                std::streamsize size = includeFile.tellg();
                includeFile.seekg(0, std::ios::beg);
                std::vector<char> fileData(size);
                includeFile.read(fileData.data(), size);
                includeFile.close();

                IDxcBlobEncoding* blob;
                dxcUtils->CreateBlob(fileData.data(), (UINT32)fileData.size(), CP_UTF8, &blob);
                *ppIncludeSource = blob;

                return S_OK;
            }
        }

        return S_FALSE;
    }

    void AddSearchDirectory(std::filesystem::path directory)
    {
        includeDirectories.push_back(directory);
    }

private:
    IDxcUtils* dxcUtils = nullptr;
    IDxcIncludeHandler* defaultHandler = nullptr;
    std::vector<std::filesystem::path> includeDirectories;
};

bool CompileShader(std::filesystem::path& file, ShaderStage& output, std::string parameters)
{
    output.data = nullptr;
    output.dataSize = 0;

#ifdef _WIN32
    std::ifstream shaderFile(file, std::ios::binary | std::ios::ate);
    std::streamsize size = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);
    std::vector<char> sourceData(size);
    shaderFile.read(sourceData.data(), size);
    shaderFile.close();

    IDxcUtils* dxUtils;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxUtils));
    IDxcBlobEncoding* sourceBlob;
    dxUtils->CreateBlob(sourceData.data(), (UINT32)sourceData.size(), CP_UTF8, &sourceBlob);

    DxcBuffer sourceBuffer;
    sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
    sourceBuffer.Size = sourceBlob->GetBufferSize();
    sourceBuffer.Encoding = 0;

    //TODO parse parameters
    std::vector<LPWSTR> additionalArguments;
    additionalArguments.push_back(L"-Qstrip_debug");
    additionalArguments.push_back(L"-Qstrip_reflect");
    additionalArguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
    additionalArguments.push_back(DXC_ARG_DEBUG);
    additionalArguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

    IDxcCompiler3* dxCompiler = nullptr;
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxCompiler));

    IDxcCompilerArgs* dxArgs = nullptr;
    dxUtils->BuildArguments(Utility::Convert(file.string()).c_str(), L"VSMain", L"vs_6_2", const_cast<LPCWSTR*>(additionalArguments.data()), static_cast<UINT32>(additionalArguments.size()), nullptr, 0, &dxArgs);

    IncludeHandler includeHandler(dxUtils);
    includeHandler.AddSearchDirectory(file.parent_path());
    //TODO pass more include directories from other modules

    IDxcResult* compilerResult;
    if (FAILED(dxCompiler->Compile(&sourceBuffer, dxArgs->GetArguments(), dxArgs->GetCount(), &includeHandler, IID_PPV_ARGS(&compilerResult))))
    {
        Utility::PrintLineD("Compilation failed");
        return false;
    }

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
    Utility::PrintLine("Compilation not support for: " + file.string() + "(" + parameters + ")");
    return false;
#endif
}

bool CompileShaders()
{
    for (std::filesystem::path& shaderFile : shaderFiles)
    {
        Shader shader;

        Utility::PrintLine(shaderFile.filename().string());

        std::string outputPath = (shaderFile.parent_path() / shaderFile.stem()).string();

        if (
            !CompileShader(shaderFile, shader.dxil.vertex, "-T vs_6_2 -E VSMain -Qstrip_reflect") ||
            !CompileShader(shaderFile, shader.dxil.pixel, "-T ps_6_2 -E PSMain -Qstrip_reflect")
            )
        {
            Utility::PrintLine("Failed to compile shader");
            return false;
        }

        //TODO compile SpirV shaders
        if (
            !CompileShader(shaderFile, shader.spirV.vertex, "-T vs_6_2 -E VSMain -spirv") ||
            !CompileShader(shaderFile, shader.spirV.pixel, "-T ps_6_2 -E PSMain -spirv")
            )
        {
            Utility::PrintLine("Failed to compile shader");
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    ParameterList parameters(argc, argv);

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    if (!ParseParameters(parameters)) return -1;

    if (!shaderFiles.empty())
    {
        if (!LoadEnvironment()) return -1;
        if (!CompileShaders()) return -1;
        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
