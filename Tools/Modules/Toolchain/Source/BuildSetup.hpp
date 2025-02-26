#pragma once
#include <string>
#include "FaroToolchainExports.generated.h"

class Toolchain;

enum FaroToolchainExports Configuration
{
    C_Debug,
    C_Development,
    C_Release,
    C_ENUMSIZE
};

extern FaroToolchainExports const char* configurationToString(Configuration configuration);
extern FaroToolchainExports Configuration configurationFromString(const char* name);

struct FaroToolchainExports Target
{
    virtual const char* displayName() const = 0;
    virtual const char* identifier() const = 0;
    virtual const char* platform() const = 0;
    virtual const Toolchain* toolchain() const = 0;
};

struct FaroToolchainExports BuildSetup
{
    Configuration configuration;
    Target* target;

    inline std::string identifier() const
    {
        return std::string(target->identifier()) + "_" + std::string(configurationToString(configuration));
    }
};
