#pragma once
#include "FaroToolchainExports.generated.h"

struct FaroToolchainExports Configuration
{
    virtual const char* name() const = 0;
    virtual const char* platform() const = 0;
    virtual const char* architecture() const = 0;
};
