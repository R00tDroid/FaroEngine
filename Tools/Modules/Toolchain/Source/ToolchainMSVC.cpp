#include "ToolchainMSVC.hpp"

ToolchainMSVC msvcInstance;

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}

unsigned int ToolchainMSVC::configurations()
{
    return 0;
}

Configuration* ToolchainMSVC::configuration(unsigned int)
{
    return nullptr;
}
