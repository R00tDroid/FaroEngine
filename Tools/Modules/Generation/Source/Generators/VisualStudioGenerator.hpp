#pragma once
#include <GeneratorInterface.hpp>
#include <vector>
#include "ModuleInfo.hpp"
#include <filesystem>
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

    virtual bool HasSourceFiles();
    virtual std::vector<std::filesystem::path> GetSourceFiles();
    virtual std::vector<std::filesystem::path> GetIncludePaths();

    virtual std::string GetBuildCommand() = 0;
    virtual std::string GetRebuildCommand() = 0;
    virtual std::string GetCleanCommand() = 0;

    virtual std::filesystem::path GetOutputExecutable(Toolchain*, Configuration*);

    virtual std::filesystem::path GetRootDirectory();

    virtual ModuleManifest* GetModuleManifest();
};

struct VSCustomCommandInfo : VSProjectInfo
{
    std::string GetBuildCommand() override;
    std::string GetRebuildCommand() override;
    std::string GetCleanCommand() override;

    std::string buildCommand, rebuildCommand, cleanCommand;
};

struct VSModuleInfo : VSProjectInfo
{
    std::string GetBuildCommand() override;

    std::string GetRebuildCommand() override;

    std::string GetCleanCommand() override;

    bool HasSourceFiles() override;

    std::vector<std::filesystem::path> GetSourceFiles() override;
    std::vector<std::filesystem::path> GetIncludePaths() override;
    std::filesystem::path GetRootDirectory() override;

    std::filesystem::path GetOutputExecutable(Toolchain*, Configuration*) override;

    ModuleManifest* GetModuleManifest() override;

    ModuleManifest* module = nullptr;
};

class VisualStudioGenerator : public GeneratorInterface
{
public:
    bool generate(const ProjectManifest* project) override;

private:
    std::string VSPlatformVersion = "";

    std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };

    void WriteProjectFile(VSProjectInfo& VSProjectInfo);

    void WriteProjectUserFile(VSProjectInfo& VSProjectInfo);

    static std::filesystem::path GetFileRelativeDirectory(std::filesystem::path root, std::filesystem::path file);

    static std::vector<std::filesystem::path> GetDirectoryTree(std::filesystem::path& root);

    void WriteFilterFile(VSProjectInfo& VSProjectInfo);

    void WriteSolutionFile(ProjectManifest* project, std::vector<VSProjectInfo*> projectInfoList);

    inline static  void WriteSolutionProjectConfig(std::ofstream& stream, VSProjectInfo& VSProjectInfo);
};
