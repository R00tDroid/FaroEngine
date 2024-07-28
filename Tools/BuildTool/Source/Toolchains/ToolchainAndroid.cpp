#define _CRT_SECURE_NO_WARNINGS
#include "ToolchainAndroid.hpp"
#include <Utility.hpp>
#include <EmscriptenInfo.hpp>


std::vector<BuildPlatform*> ToolchainAndroid::GetPlatforms()
{
    return {};
}

bool ToolchainAndroid::PrepareModuleForBuild(ModuleManifest&, BuildPlatform*, BuildType)
{
    return false;
}

bool ToolchainAndroid::BuildSource(ModuleManifest&, BuildPlatform*, BuildType, std::filesystem::path, std::vector<std::filesystem::path>, std::vector<std::string>)
{
    return false;
}

bool ToolchainAndroid::LinkLibrary(ModuleManifest&, BuildPlatform*, BuildType, std::vector<std::filesystem::path>)
{
    return false;
}

bool ToolchainAndroid::LinkExecutable(ModuleManifest&, BuildPlatform*, BuildType, std::vector<std::filesystem::path>)
{
    return false;
}

std::string ToolchainAndroid::GetObjExtension()
{
    return "o";
}

std::string ToolchainAndroid::GetLibExtension()
{
    return "a";
}

std::string ToolchainAndroid::GetExeExtension()
{
    return "so";
}

std::vector<std::filesystem::path> ToolchainAndroid::GetToolchainIncludes(BuildPlatform*, BuildType)
{
    return {};
}
