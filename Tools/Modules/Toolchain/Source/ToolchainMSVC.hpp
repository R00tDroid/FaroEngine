#pragma once
#include "Toolchain.hpp"
#include "BuildSetup.hpp"

struct TargetMSVC : Target
{
    TargetMSVC(const char* configName, const char* configId) : configName(configName), configId(configId) {}

    const char* configName;
    const char* configId;

    const char* platform() const override { return "windows"; }
    const char* displayName() const override { return configName;  }
    const char* identifier() const override { return configId; }
};

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();

    unsigned targets() override;
    Target* target(unsigned index) override;
};
