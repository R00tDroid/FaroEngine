#pragma once
#include "IToolchain.hpp"
#include <Manifests/ModuleManifest.hpp>

enum EMSVCArchitecture 
{
    x64,
    x86
};

struct MSVCBuildPlatform : BuildPlatform
{
    EMSVCArchitecture Architecture;
};

class ToolchainMSVC : public IToolchain
{
public:
    static ToolchainMSVC Instance;

    std::vector<BuildPlatform*> GetPlatforms() override;

    bool PrepareModuleForBuild(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration) override;

    bool BuildSource(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines) override;

    bool LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) override;

    bool LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) override;

    std::string GetObjExtension() override;

    std::string GetLibExtension() override;

    std::string GetExeExtension() override;

    std::vector<std::filesystem::path> GetToolchainIncludes(BuildPlatform* platform, BuildType configuration) override;

private:
    std::string GetEnvCommand();
    std::filesystem::path objDir = "";

    std::filesystem::path msvcRoot = "";
    std::filesystem::path msvcTools = "";

    std::filesystem::path windowsSdkInclude = "";
    std::filesystem::path windowsSdkLib = "";

    std::vector<BuildPlatform*> platforms;
};

inline ToolchainMSVC ToolchainMSVC::Instance;
