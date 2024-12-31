#pragma once
#include <vector>
#include "FaroToolchainExports.generated.h"

class FaroToolchainExports Toolchain
{
public:
    inline static std::vector<Toolchain*> getToolchains()
    {
        return {}; //TODO Instantiate toolchains
    }
};
