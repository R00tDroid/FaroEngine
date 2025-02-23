#pragma once
#include <GeneratorInterface.hpp>
#include <vector>
#include "ModuleInfo.hpp"
#include <filesystem>
#include "MSVCInfo.hpp"
#include "Toolchain.hpp"

struct VSProjectInfo
{
    std::string name;
    std::filesystem::path projectPath;
    std::string uuid;
    bool buildByDefault = false;
    std::filesystem::path solutionPath;
    bool debuggable = false;

    std::vector<std::string> dependencyUuids;

    virtual bool hasSourceFiles();
    virtual std::vector<std::filesystem::path> getSourceFiles() const;
    virtual std::vector<std::filesystem::path> getIncludePaths() const;

    virtual std::string getBuildCommand() const = 0;
    virtual std::string getRebuildCommand() const = 0;
    virtual std::string getCleanCommand() const = 0;

    virtual std::filesystem::path getOutputExecutable(const Toolchain*, const BuildSetup&) const;

    virtual std::filesystem::path getRootDirectory() const;

    virtual ModuleManifest* getModuleManifest() const;
};

struct VSCustomCommandInfo : VSProjectInfo
{
    std::string getBuildCommand() const override;
    std::string getRebuildCommand() const override;
    std::string getCleanCommand() const override;

    std::string buildCommand, rebuildCommand, cleanCommand;
};

struct VSModuleInfo : VSProjectInfo
{
    std::string getBuildCommand() const override;

    std::string getRebuildCommand() const override;

    std::string getCleanCommand() const override;

    bool hasSourceFiles() override;

    std::vector<std::filesystem::path> getSourceFiles() const override;
    std::vector<std::filesystem::path> getIncludePaths() const override;
    std::filesystem::path getRootDirectory() const override;

    std::filesystem::path getOutputExecutable(const Toolchain*, const BuildSetup&) const override;

    ModuleManifest* getModuleManifest() const override;

    ModuleManifest* module = nullptr;
};

class VisualStudioGenerator : public GeneratorInterface
{
public:
    bool generate(const ProjectManifest* project) override;

private:
    std::string vsPlatformVersion = "";

    std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };

    void writeProjectFile(const VSProjectInfo& vsProjectInfo);

    void writeProjectUserFile(const VSProjectInfo& vsProjectInfo);

    static std::filesystem::path getFileRelativeDirectory(std::filesystem::path root, std::filesystem::path file);

    static std::vector<std::filesystem::path> getDirectoryTree(std::filesystem::path& root);

    void writeFilterFile(const VSProjectInfo& vsProjectInfo);

    void writeSolutionFile(const ProjectManifest* project, std::vector<VSProjectInfo*> projectInfoList);

    inline static void writeSolutionProjectConfig(std::ofstream& stream, const VSProjectInfo& vsProjectInfo);

    MSVCVersion msvcVersion;
};
