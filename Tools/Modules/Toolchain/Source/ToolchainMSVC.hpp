#pragma once
#include "Toolchain.hpp"

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();
};
