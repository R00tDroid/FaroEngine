#pragma once
#include <filesystem>
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
    Toolchain* toolchain() const override;
};

class ToolchainMSVC : public Toolchain
{
public:
    static ToolchainMSVC& instance();

    ToolchainMSVC();

    unsigned targets() override;
    Target* target(unsigned index) override;
    bool prepare(const BuildSetup& buildSetup) override;
    bool compile(const ToolchainCompileInfo& info) const override;

private:
    MSVCVersion msvcVersion;
    std::vector<TargetMSVC> configurations;

    std::vector<std::string> includePaths;
    std::vector<std::string> defines;

    std::filesystem::path msvcRoot;
    std::filesystem::path msvcTools;

    std::filesystem::path clExe;
    std::filesystem::path msvcDrive;

    std::filesystem::path windowsSdkInclude;
    std::filesystem::path windowsSdkLib;
};
