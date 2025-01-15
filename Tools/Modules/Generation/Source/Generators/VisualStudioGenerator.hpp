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

    virtual bool hasSourceFiles();
    virtual std::vector<std::filesystem::path> getSourceFiles();
    virtual std::vector<std::filesystem::path> getIncludePaths();

    virtual std::string getBuildCommand() = 0;
    virtual std::string getRebuildCommand() = 0;
    virtual std::string getCleanCommand() = 0;

    virtual std::filesystem::path getOutputExecutable(Toolchain*, Configuration*);

    virtual std::filesystem::path getRootDirectory();

    virtual ModuleManifest* getModuleManifest();
};

struct VSCustomCommandInfo : VSProjectInfo
{
    std::string getBuildCommand() override;
    std::string getRebuildCommand() override;
    std::string getCleanCommand() override;

    std::string buildCommand, rebuildCommand, cleanCommand;
};

struct VSModuleInfo : VSProjectInfo
{
    std::string getBuildCommand() override;

    std::string getRebuildCommand() override;

    std::string getCleanCommand() override;

    bool hasSourceFiles() override;

    std::vector<std::filesystem::path> getSourceFiles() override;
    std::vector<std::filesystem::path> getIncludePaths() override;
    std::filesystem::path getRootDirectory() override;

    std::filesystem::path getOutputExecutable(Toolchain*, Configuration*) override;

    ModuleManifest* getModuleManifest() override;

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
};
