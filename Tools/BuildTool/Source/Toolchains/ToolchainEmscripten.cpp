#include "ToolchainEmscripten.hpp"
#include <Utility.hpp>

std::vector<BuildPlatform*> ToolchainEmscripten::GetPlatforms()
{
    if (platforms.empty())
    {
        //TODO Prevent double "Web Web"
        platforms.push_back(new EmscriptenBuildPlatform{ "Web web", { "FARO_OS_WEB", "FARO_ARCH_WEB" }, {} });
    }

    return platforms;
}

bool ToolchainEmscripten::PrepareModuleForBuild(ModuleManifest&, BuildPlatform*, BuildType)
{
    //TODO Locate Emcripten SDK
    return false;
}

#define MEMORY_SIZE 512 * 1024 * 1024

bool ToolchainEmscripten::BuildSource(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines)
{
    std::string includes = "";

    includes += " -I\"" + (emscriptenRoot / "upstream" / "emscripten" / "system" / "include").string() + "\"";

    for (std::filesystem::path& include : includePaths)
    {
        includes += " -I\"" + include.string() + "\"";
    }

    std::string defines;
    for (std::string& define : preprocessorDefines)
    {
        defines += " -D" + define;
    }

    std::string compilerFlags;

    compilerFlags += " -std=c++11";
    compilerFlags += " -s USE_PTHREADS=1";
    compilerFlags += " -s INITIAL_MEMORY=" + std::to_string(MEMORY_SIZE);

    switch (configuration)
    {
    case Debug:
    case Development: { compilerFlags += " /O0 /g"; break; }
    case Release: { compilerFlags += " /O3"; break; }

    case ENUMSIZE:;
    default:;
    }

    std::filesystem::path outputFile = GetObjPath(manifest, target, configuration, sourceFile);
    Utility::EnsureDirectory(outputFile.parent_path());

    std::filesystem::path env_path = emscriptenRoot / "emsdk_env.bat";


    std::string log = "";
    int result = ExecuteCommand(env_path.string() + "  >nul 2>&1 && em++ -c " + sourceFile.string() + compilerFlags + includes + defines + " -o " + outputFile.string(), log);

    // Format, trim and print output message
    if (!log.empty())
    {
        log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

        std::string header = sourceFile.filename().string() + "\n";
        if (log.substr(0, header.size()) == header)
        {
            log = log.substr(header.size());
        }

        if (!log.empty())
        {
            Utility::PrintLine(log);
        }
    }

    return result == 0;
}

bool ToolchainEmscripten::LinkLibrary(ModuleManifest&, BuildPlatform*, BuildType,  std::vector<std::filesystem::path>)
{
    //TODO Implement
    return false;
}

bool ToolchainEmscripten::LinkExecutable(ModuleManifest&, BuildPlatform*, BuildType, std::vector<std::filesystem::path>)
{
    //TODO Implement
    return false;
}

std::string ToolchainEmscripten::GetObjExtension()
{
    //TODO Implement
    return {};
}

std::string ToolchainEmscripten::GetLibExtension()
{
    //TODO Implement
    return {};
}

std::string ToolchainEmscripten::GetExeExtension()
{
    //TODO Implement
    return {};
}
