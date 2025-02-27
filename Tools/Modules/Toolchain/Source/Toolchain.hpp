#pragma once
#include "BuildSetup.hpp"
#include "FaroToolchainExports.generated.h"

class ModuleManifest;
struct Target;

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

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);

    virtual unsigned int targets() = 0;
    virtual Target* target(unsigned int index) = 0;

    virtual bool prepare(const BuildSetup& buildSetup) = 0;
    virtual bool compile(const ToolchainCompileInfo& info) const = 0;
};

extern FaroToolchainExports unsigned int buildSetups();
extern FaroToolchainExports BuildSetup buildSetup(unsigned int);
