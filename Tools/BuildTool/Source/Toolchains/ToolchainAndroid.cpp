#include "ToolchainAndroid.hpp"
#include <Utility.hpp>

std::vector<BuildPlatform*> ToolchainAndroid::GetPlatforms()
{
    if (platforms.empty())
    {
        platforms.push_back(new AndroidBuildPlatform{ "Android", { "FARO_OS_ANDROID", "FARO_ARCH_ARM" }, {} });
    }

    return platforms;
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
