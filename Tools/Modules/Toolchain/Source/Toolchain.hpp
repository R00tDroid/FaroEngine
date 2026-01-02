#pragma once
#include "BuildSetup.hpp"
#include <filesystem>
#include "FaroToolchainExports.generated.h"

class ModuleManifest;
struct Target;

enum LinkType
{
    LT_StaticLibrary,
    LT_Application,
};

struct FaroToolchainExports ToolchainCompileInfo
{
    const BuildSetup& buildSetup;
    const char* file = nullptr;
    const char* output = nullptr;

    unsigned int includePaths = 0;
    const char** includePathsPtr = nullptr;

    unsigned int defines = 0;
    const char** definesPtr = nullptr;

    void* userData = nullptr;
    void (*onLog)(void*, unsigned int, const char*) = nullptr;
};

struct FaroToolchainExports ToolchainLinkInfo
{
    const BuildSetup& buildSetup;
    LinkType linkType;

    unsigned int objFiles = 0;
    const char** objFilesPtr = nullptr;

    unsigned int moduleLibs = 0;
    const char** moduleLibsPtr = nullptr;

    unsigned int linkLibs = 0;
    const char** linkLibsPtr = nullptr;

    unsigned int libPaths = 0;
    const char** libPathsPtr = nullptr;

    const char* output = nullptr;

    void* userData = nullptr;
    void (*onLog)(void*, unsigned int, const char*) = nullptr;
};

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);

    virtual unsigned int targets() = 0;
    virtual Target* target(unsigned int index) = 0;

    virtual bool prepare(const BuildSetup& buildSetup) = 0;
    virtual bool compile(const ToolchainCompileInfo& info) const = 0;
    virtual bool link(const ToolchainLinkInfo& info) const = 0;

    virtual const char* getBinExtension() const = 0;
    virtual const char* getLinkExtension(LinkType type) const = 0;

    virtual unsigned includePaths() const = 0;
    virtual const char* includePath(unsigned index) const = 0;

    static std::vector<std::string> getSetupDefines(const BuildSetup& buildSetup) {
        std::vector<std::string> defineStrings;

        for (unsigned int i = 0; i < defines(buildSetup); i++)
        {
            defineStrings.push_back(define(buildSetup, i));
        }

        for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
        {
            Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
            for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
            {
                Target* target = toolchain->target(targetIndex);
                bool isCurrent = target == buildSetup.target;

                for (unsigned int defineIndex = 0; defineIndex < target->defines(); defineIndex++)
                {
                    std::string define = target->define(defineIndex);
                    defineStrings.push_back(define + "=" + (isCurrent ? "1" : "0"));
                }
            }
        }

        return defineStrings;
    }

private:
    static unsigned int defines(const BuildSetup& buildSetup);
    static const char* define(const BuildSetup& buildSetup, unsigned int index);
};

extern FaroToolchainExports unsigned int buildSetups();
extern FaroToolchainExports BuildSetup buildSetup(unsigned int);
