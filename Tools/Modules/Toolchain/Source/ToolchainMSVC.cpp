#include "ToolchainMSVC.hpp"

TargetMSVC* msvcConfigurations[] = {
    new TargetMSVC("Windows", "windowsx64"),
};

ToolchainMSVC msvcInstance;

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}

unsigned int ToolchainMSVC::targets()
{
    return sizeof(msvcConfigurations) / sizeof(TargetMSVC*);
}

Target* ToolchainMSVC::target(unsigned int index)
{
    return msvcConfigurations[index];
}
