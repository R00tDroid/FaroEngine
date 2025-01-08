#include "Toolchain.hpp"
#include <vector>
#include "ToolchainMSVC.hpp"

std::vector<Toolchain*> toolchainList()
{
    return std::vector<Toolchain*> {
        &ToolchainMSVC::instance()
    };
}

unsigned int Toolchain::toolchains()
{
    return static_cast<unsigned int>(toolchainList().size());
}

Toolchain* Toolchain::toolchain(unsigned int index)
{
    return toolchainList()[index];
}
