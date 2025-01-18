#pragma once
#include "Toolchain.hpp"
#include "BuildSetup.hpp"

struct TargetMSVC : Target
{
    const char* configName;
    const char* configArchitecture;

    const char* name() const override { return configName; }
    const char* platform() const override { return nullptr; }
    const char* architecture() const override { return configArchitecture; }
};

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();

    unsigned targets() override;
    Target* target(unsigned index) override;
};
