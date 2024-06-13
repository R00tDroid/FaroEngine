#pragma once
#include <Utility.hpp>
#include <ShaderDefinition.hpp>
#include "ShaderTranspilerDll.h"

#ifdef _WIN32
#include <Windows.h>
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#endif

class ShaderTranspilerExport ShaderCompiler
#ifdef _WIN32
    : public IDxcIncludeHandler
#endif
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

    ShaderCompiler();
    virtual ~ShaderCompiler();

#ifdef _WIN32
    HRESULT QueryInterface(const IID& riid, void** ppvObject) override;
    ULONG AddRef() override { return 0; }
    ULONG Release() override { return 0; }
    HRESULT LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource) override;
#endif

    void SetSearchDirectory(std::vector<std::filesystem::path>& directories);

    bool CompileShader(std::filesystem::path& file, ShaderStage& output, CompilerFlavor compilerFlavor, ShaderType shaderStage, bool debug = false);

private:
#ifdef _WIN32
    IDxcUtils* dxcUtils = nullptr;
    IDxcIncludeHandler* defaultHandler = nullptr;
    IDxcBlobEncoding* LoadFileToBlob(std::filesystem::path& filePath);
#endif

    // List of directories to use for include searching
    //std::vector<std::filesystem::path> searchDirectories;

    // searchDirectories + parent folder of currently compiled shader file
    //std::vector<std::filesystem::path> includeDirectories;
};
