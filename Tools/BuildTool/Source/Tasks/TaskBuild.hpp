#pragma once
#include <string>
#include "ITask.hpp"
#include <Manifests/ModuleManifest.hpp>

class TaskBuild : public ITask
{
public:
    TaskBuild(std::string platform, BuildType configuration);

    int GetPriority() const override;

    static std::string GetDisplayName(ModuleManifest& module, std::filesystem::path& filePath);

    bool Run(TaskRunInfo& runInfo) override;

    std::string GetTaskName() override { return "Build"; }

private:
    struct ModuleOrderInfo
    {
        ModuleManifest* module = nullptr;
        std::set<ModuleManifest*> dependencies;
        int orderIndex = 0;
    };

    bool FindToolchain();

    bool CheckModule(ModuleOrderInfo& moduleInfo);

    bool BuildModule(ModuleManifest* module);

    bool CompileModule(ModuleManifest* module, std::vector<std::filesystem::path>& filesToCompile);

    bool LinkModule(ModuleManifest* module, std::vector<std::filesystem::path>& sourceFiles);

    std::vector<std::filesystem::path> FindFilesToCompile(ModuleManifest* module, std::vector<std::filesystem::path>& outSourceFiles);

    int GetModuleOrderIndex(ModuleOrderInfo& orderInfo, std::map<ModuleManifest*, ModuleOrderInfo>& orderMap);

private:
    std::string buildPlatform;
    BuildType buildType;

    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
};
