#pragma once
#include "IToolchain.hpp"
#include <Manifests/ModuleManifest.hpp>

typedef BuildPlatform AndroidBuildPlatform;

class ToolchainAndroid : public IToolchain
{
public:
    static ToolchainAndroid Instance;

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
    std::vector<BuildPlatform*> platforms;
    std::filesystem::path sdkRoot = "D:\\Content\\AndroidSDK"; //TODO Find NDK dynamically
    std::string ndkVersion = "25.2.9519653"; //TODO Find NDK version dynamically

    std::filesystem::path ndkToolchain;
    std::filesystem::path ndkBin;
};

inline ToolchainAndroid ToolchainAndroid::Instance;
