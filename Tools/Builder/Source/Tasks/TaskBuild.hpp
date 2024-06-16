#pragma once
#include <string>
#include "ITask.hpp"
#include <Manifests/ModuleManifest.hpp>

class TaskBuild : public ITask
{
public:
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

    bool FindToolchain(const std::string& buildPlatform);

    bool CheckModule(BuildType buildType, ModuleOrderInfo& moduleInfo);

    bool BuildModule(BuildType buildType, ModuleManifest* module);

    bool CompileModule(BuildType buildType, ModuleManifest* module, std::vector<std::filesystem::path>& filesToCompile);

    bool LinkModule(BuildType buildType, ModuleManifest* module, std::vector<std::filesystem::path>& sourceFiles);

    std::vector<std::filesystem::path> FindFilesToCompile(BuildType buildType, ModuleManifest* module, std::vector<std::filesystem::path>& outSourceFiles);

    int GetModuleOrderIndex(ModuleOrderInfo& orderInfo, std::map<ModuleManifest*, ModuleOrderInfo>& orderMap);

private:
    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
};
