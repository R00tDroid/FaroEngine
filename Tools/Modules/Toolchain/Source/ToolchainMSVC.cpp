#include "ToolchainMSVC.hpp"

ToolchainMSVC msvcInstance;

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}
