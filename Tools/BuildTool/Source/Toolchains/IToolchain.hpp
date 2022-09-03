#pragma once
#include <filesystem>
#include <string>
#include <vector>

class ModuleManifest;

enum BuildType
{
    Debug,
    Development,
    Release,

    ENUMSIZE
};

inline const char* BuildTypeNames[] =
{
    "Debug",
    "Development",
    "Release",
};

struct BuildPlatform
{
    std::string platformName;
    std::vector<std::string> preprocessorDefines;
    std::vector<std::string> linkerLibraries;
};

class IToolchain
{
public:
    static std::vector<IToolchain*> GetToolchains();

    virtual ~IToolchain() = default;

    virtual std::vector<BuildPlatform*> GetPlatforms() = 0;

    virtual bool PrepareModuleForBuild(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration) = 0;

    virtual bool BuildSource(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines) = 0;

    virtual bool LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) = 0;

    virtual bool LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles) = 0;

    std::filesystem::path GetObjDirectory(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration);

    std::filesystem::path GetObjPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile);

    virtual std::string GetObjExtension() = 0;

    std::filesystem::path GetBinDirectory(ModuleManifest& manifest, BuildType configuration);

    std::filesystem::path GetBinPath(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration);

    virtual std::string GetLibExtension() = 0;

    virtual  std::string GetExeExtension() = 0;

protected:
    static int ExecuteCommand(std::string command, std::string& output);

    static int ExecuteCommand(std::string command);
};
