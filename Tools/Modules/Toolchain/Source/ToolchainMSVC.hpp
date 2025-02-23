#pragma once
#include <vector>
#include "Toolchain.hpp"
#include "BuildSetup.hpp"

class ToolchainMSVC;

struct TargetMSVC : Target
{
    TargetMSVC(const char* configName, const char* configId, ToolchainMSVC* msvcToolchain);

    const char* configName;
    const char* configId;
    ToolchainMSVC* msvcToolchain = nullptr;

    const char* platform() const override;
    const char* displayName() const override;
    const char* identifier() const override;
    const Toolchain* toolchain() const override;
};

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();

    ToolchainMSVC();

    unsigned targets() override;
    Target* target(unsigned index) override;

private:
    std::vector<TargetMSVC> configurations;
};
