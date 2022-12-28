#pragma once
#include <Utility.hpp>
#include <Shader.hpp>
#include "VulkanSDKInfo.hpp"

#ifdef _WIN32
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#endif

class ShaderCompiler : public IDxcIncludeHandler
{
public:
    enum CompilerFlavor
    {
        CF_DXIL,
        CF_SpirV
    };
    enum ShaderType
    {
        SS_Vertex,
        SS_Pixel
    };

    ShaderCompiler(IDxcUtils* dxcUtils);
    virtual ~ShaderCompiler();

    HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
    ULONG AddRef() override { return 0; }
    ULONG Release() override { return 0; }
    HRESULT LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource) override;

    void SetSearchDirectory(std::vector<std::filesystem::path>& directories);

    bool CompileShader(std::filesystem::path& file, ShaderStage& output, CompilerFlavor compilerFlavor, ShaderType shaderStage, bool debug = false);

private:
    IDxcUtils* dxcUtils = nullptr;
    IDxcIncludeHandler* defaultHandler = nullptr;

    // List of directories to use for include searching
    std::vector<std::filesystem::path> searchDirectories;

    // searchDirectories + parent folder of currently compiled shader file
    std::vector<std::filesystem::path> includeDirectories;

    IDxcBlobEncoding* LoadFileToBlob(std::filesystem::path& filePath);
};
