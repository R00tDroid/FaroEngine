#pragma once
#include "FaroToolchainExports.generated.h"

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);
};
