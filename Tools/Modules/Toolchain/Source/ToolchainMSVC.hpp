#pragma once
#include <vector>
#include "Toolchain.hpp"
#include "BuildSetup.hpp"
#include "MSVCInfo/MSVCInfo.hpp"

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
    bool compile(const BuildSetup& buildSetup, const char* file) const override;

private:
    MSVCVersion msvcVersion;
    std::vector<TargetMSVC> configurations;
};
