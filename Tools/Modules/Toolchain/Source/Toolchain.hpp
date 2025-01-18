#pragma once
#include "FaroToolchainExports.generated.h"

struct Target;

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);

    virtual unsigned int targets() = 0;
    virtual Target* target(unsigned int index) = 0;
};
