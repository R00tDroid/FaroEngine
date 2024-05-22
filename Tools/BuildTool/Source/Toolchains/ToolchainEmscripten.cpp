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

std::string GetEmscriptenFlags()
{
    std::string compilerFlags;
    compilerFlags += " -std=c++11";
    compilerFlags += " -s USE_PTHREADS=1";
    compilerFlags += " -s INITIAL_MEMORY=" + std::to_string(MEMORY_SIZE);
    return compilerFlags;
}

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

    std::string compilerFlags = GetEmscriptenFlags();

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
    int result = ExecuteCommand(env_path.string() + " >nul 2>&1 && em++ -c " + sourceFile.string() + compilerFlags + includes + defines + " -o " + outputFile.string(), log);

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

bool ToolchainEmscripten::LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles)
{
    std::string objs = "";
    for (std::filesystem::path& sourceFile : sourceFiles)
    {
        objs += " \"" + GetObjPath(manifest, target, configuration, sourceFile).string() + "\"";
    }

    std::filesystem::path archiver = emscriptenRoot / "upstream" / "bin" / "llvm-ar.exe";

    std::filesystem::path libPath = GetLibPath(manifest, target, configuration);
    Utility::EnsureDirectory(libPath.parent_path());
    Utility::PrintLineD(libPath.string());

    std::string log = "";
    int result = ExecuteCommand(archiver.string() + " crsD \"" + libPath.string() + "\" " + objs, log);

    // Format, trim and print output message
    if (!log.empty())
    {
        log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

        if (!log.empty())
        {
            Utility::PrintLine(log);
        }
    }

    return result == 0;
}

bool ToolchainEmscripten::LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles)
{
    // Get project object files
    std::string objs = "";
    for (std::filesystem::path& sourceFile : sourceFiles)
    {
        objs += " \"" + GetObjPath(manifest, target, configuration, sourceFile).string() + "\"";
    }

    std::string libs = "";
    std::string moduleLibs = "";

    // Get dependency libraries
    for (ModuleManifest* module : GetAllModuleDependencies(manifest))
    {
        std::filesystem::path lib = GetLibPath(*module, target, configuration);
        Utility::PrintLineD("\t" + lib.string());
        moduleLibs += " /WHOLEARCHIVE:\"" + lib.string() + "\"";

        for (std::string& linkerLibrary : module->linkingLibraries)
        {
            libs += " \"" + linkerLibrary + "\"";
        }
    }

    std::filesystem::path outputFile = GetExePath(manifest, target, configuration);
    Utility::EnsureDirectory(outputFile.parent_path());
    Utility::PrintLineD(outputFile.string());

    std::string compilerFlags = GetEmscriptenFlags();

    std::filesystem::path env_path = emscriptenRoot / "emsdk_env.bat";

    std::string log = "";
    int result = ExecuteCommand(env_path.string() + "  >nul 2>&1 && em++ " + compilerFlags + objs + libs + moduleLibs + " -o " + outputFile.string(), log);

    // Format, trim and print output message
    if (!log.empty())
    {
        log.erase(std::remove(log.begin(), log.end(), '\r'), log.end());

        if (!log.empty())
        {
            Utility::PrintLine(log);
        }
    }
    return result == 0;
}

std::string ToolchainEmscripten::GetObjExtension()
{
    return "o";
}

std::string ToolchainEmscripten::GetLibExtension()
{
    return "a";
}

std::string ToolchainEmscripten::GetExeExtension()
{
    return "js";
}
