#pragma once
#include "FaroToolchainExports.generated.h"

struct Configuration;

class FaroToolchainExports Toolchain
{
public:
    static unsigned int toolchains();
    static Toolchain* toolchain(unsigned int index);

    virtual unsigned int configurations() = 0;
    virtual Configuration* configuration(unsigned int index) = 0;
};
