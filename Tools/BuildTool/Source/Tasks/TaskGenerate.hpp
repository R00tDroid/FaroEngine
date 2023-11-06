#pragma once
#include <filesystem>
#include "ITask.hpp"
#include "MSVCInfo.hpp"
#include "Toolchains/IToolchain.hpp"
#include "Manifests/ModuleManifest.hpp"

class TaskGenerate : public ITask
{
public:
    struct ProjectInfo
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

        virtual std::filesystem::path GetOutputExecutable(IToolchain*, BuildPlatform*, BuildType);

        virtual std::filesystem::path GetRootDirectory();
    };

    struct CustomCommandInfo : ProjectInfo
    {
        std::string GetBuildCommand() override;
        std::string GetRebuildCommand() override;
        std::string GetCleanCommand() override;

        std::string buildCommand, rebuildCommand, cleanCommand;
    };

    struct ModuleInfo : ProjectInfo
    {
        std::string GetBuildCommand() override;

        std::string GetRebuildCommand() override;

        std::string GetCleanCommand() override;

        bool HasSourceFiles() override;

        std::vector<std::filesystem::path> GetSourceFiles() override;
        std::vector<std::filesystem::path> GetIncludePaths() override;
        std::filesystem::path GetRootDirectory() override;

        std::filesystem::path GetOutputExecutable(IToolchain* toolchain, BuildPlatform* platform, BuildType type) override;

        ModuleManifest* module = nullptr;
    };

    int GetPriority() const override;

    bool Run(TaskRunInfo& taskInfo) override;

    std::string GetTaskName() override { return "Generate"; }

private:
    std::string VSPlatformVersion = "";

    std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };

    void WriteProjectFile(ProjectInfo& projectInfo);

    void WriteProjectUserFile(ProjectInfo& projectInfo);

    static std::filesystem::path GetFileRelativeDirectory(std::filesystem::path root, std::filesystem::path file);

    static std::vector<std::filesystem::path> GetDirectoryTree(std::filesystem::path& root);

    void WriteFilterFile(ProjectInfo& projectInfo);

    void WriteSolutionFile(ProjectManifest* project, std::vector<ProjectInfo*> projectInfoList);

    inline static  void WriteSolutionProjectConfig(std::ofstream& stream, ProjectInfo& projectInfo);
};
