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
    ndkToolchain = sdkRoot / "ndk" / ndkVersion / "toolchains" / "llvm" / "prebuilt" / "windows-x86_64";
    ndkBin = ndkToolchain / "bin";
    return true;
}

std::string getClangFlags()
{
    return " -std=c++11";
}

bool ToolchainAndroid::BuildSource(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::filesystem::path sourceFile, std::vector<std::filesystem::path> includePaths, std::vector<std::string> preprocessorDefines)
{
    std::string includes = "";

    for (std::filesystem::path& include : includePaths)
    {
        includes += " -I\"" + include.string() + "\"";
    }

    std::string defines;
    for (std::string& define : preprocessorDefines)
    {
        defines += " -D" + define;
    }

    std::string compilerFlags = getClangFlags();

    switch (configuration)
    {
    case Debug:
    case Development: { compilerFlags += " -O0 -g"; break; }
    case Release: { compilerFlags += " -O3"; break; }

    case ENUMSIZE:;
    default:;
    }

    std::filesystem::path outputFile = GetObjPath(manifest, target, configuration, sourceFile);
    Utility::EnsureDirectory(outputFile.parent_path());

    std::filesystem::path clang = ndkBin / "armv7a-linux-androideabi26-clang++"; //TODO Find clang variant dynamically

    std::string log = "";
    int result = ExecuteCommand(clang.string() + " -c " + sourceFile.string() + compilerFlags + includes + defines + " -o " + outputFile.string(), log);

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
            //TODO Rewrite file line numbers
            Utility::PrintLine(log);
        }
    }

    return result == 0;
}

bool ToolchainAndroid::LinkLibrary(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles)
{
    std::string objs = "";
    for (std::filesystem::path& sourceFile : sourceFiles)
    {
        objs += " \"" + GetObjPath(manifest, target, configuration, sourceFile).string() + "\"";
    }

    std::filesystem::path archiver = ndkBin / "llvm-ar.exe";

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

bool ToolchainAndroid::LinkExecutable(ModuleManifest& manifest, BuildPlatform* target, BuildType configuration, std::vector<std::filesystem::path> sourceFiles)
{
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
        if (!module->IsCompatible(target)) continue;

        std::filesystem::path lib = GetLibPath(*module, target, configuration);
        Utility::PrintLineD("\t" + lib.string());
        moduleLibs += " -Wl \"" + lib.string() + "\"";

        for (std::string& linkerLibrary : module->linkingLibraries)
        {
            libs += " \"" + linkerLibrary + "\"";
        }
    }

    std::filesystem::path clang = ndkBin / "armv7a-linux-androideabi26-clang++"; //TODO Find variant

    std::filesystem::path libPath = GetExePath(manifest, target, configuration);
    Utility::EnsureDirectory(libPath.parent_path());
    Utility::PrintLineD(libPath.string());

    std::string log = "";
    int result = ExecuteCommand(clang.string() + " " + objs + " " + moduleLibs + " " + libs + " -o \"" + libPath.string() + "\"", log);

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
