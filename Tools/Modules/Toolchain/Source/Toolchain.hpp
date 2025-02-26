#pragma once
#include "BuildSetup.hpp"
#include "FaroToolchainExports.generated.h"

class ModuleManifest;
struct Target;

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);

    virtual unsigned int targets() = 0;
    virtual Target* target(unsigned int index) = 0;

    virtual bool compile(const BuildSetup& buildSetup, const char* file) const = 0;
};

extern FaroToolchainExports unsigned int buildSetups();
extern FaroToolchainExports BuildSetup buildSetup(unsigned int);
