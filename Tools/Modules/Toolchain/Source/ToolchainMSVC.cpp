#include "ToolchainMSVC.hpp"

TargetMSVC* msvcConfigurations[2] = {
    new TargetMSVC("WindowsX64", "x64"),
    new TargetMSVC(),
};

ToolchainMSVC msvcInstance;

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}

unsigned int ToolchainMSVC::targets()
{
    return 0;
}

Target* ToolchainMSVC::target(unsigned int)
{
    return nullptr;
}
