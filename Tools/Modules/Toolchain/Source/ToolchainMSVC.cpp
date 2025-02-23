#include "ToolchainMSVC.hpp"

ToolchainMSVC msvcInstance;

TargetMSVC::TargetMSVC(const char* configName, const char* configId, ToolchainMSVC* msvcToolchain): configName(configName), configId(configId), msvcToolchain(msvcToolchain) {}

const char* TargetMSVC::platform() const { return "windows"; }

const char* TargetMSVC::displayName() const { return configName; }

const char* TargetMSVC::identifier() const { return configId; }

const Toolchain* TargetMSVC::toolchain() const { return msvcToolchain; }

ToolchainMSVC& ToolchainMSVC::instance()
{
    return msvcInstance;
}

ToolchainMSVC::ToolchainMSVC()
{
    configurations.push_back(TargetMSVC("Windows", "windowsx64", this));
}

unsigned int ToolchainMSVC::targets()
{
    return static_cast<unsigned int>(configurations.size());
}

Target* ToolchainMSVC::target(unsigned int index)
{
    return &configurations[index];
}
