#pragma once
#include "FaroToolchainExports.generated.h"

enum FaroToolchainExports Configuration
{
    C_Debug,
    C_Development,
    C_Release,
    C_ENUMSIZE
};


struct FaroToolchainExports Target
{
    virtual const char* displayName() const = 0;
    virtual const char* identifier() const = 0;
    virtual const char* platform() const = 0;
    virtual const char* architecture() const = 0;
};

struct FaroToolchainExports BuildSetup
{
    Configuration configuration;
    Target* target;
};
