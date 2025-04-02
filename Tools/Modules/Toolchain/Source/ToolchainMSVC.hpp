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

    const char* displayName() const override;
    const char* identifier() const override;
    Toolchain* toolchain() const override;

    unsigned int defines() const override;
    const char* define(unsigned int index) const override;

    static std::vector<const char*> msvcDefines;
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
    bool link(const ToolchainLinkInfo& info) const override;
    char* getBinExtension() const override;
    char* getLinkExtension(LinkType type) const override;

private:
    MSVCVersion msvcVersion;
    std::vector<TargetMSVC> configurations;

    std::vector<std::string> includePaths;
    std::vector<std::string> defines;

    std::filesystem::path msvcRoot;
    std::filesystem::path msvcTools;

    std::filesystem::path clExe;
    std::filesystem::path libExe;
    std::filesystem::path linkExe;
    std::filesystem::path msvcDrive;

    std::filesystem::path windowsSdkInclude;
    std::filesystem::path windowsSdkLib;
};
