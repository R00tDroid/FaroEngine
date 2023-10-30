#pragma once
#include <string>
#include "ITask.hpp"
#include <Manifests/ModuleManifest.hpp>

class TaskBuild : public ITask
{
public:
    TaskBuild(std::string platform, std::string architecture, BuildType configuration);

    int GetPriority() const override;

    static std::string GetDisplayName(ModuleManifest& module, std::filesystem::path& filePath);

    bool Run(TaskRunInfo& runInfo) override;

private:
    bool FindToolchain();

    bool BuildModule(ModuleManifest* module);

    bool CompileModule(ModuleManifest* module, std::vector<std::filesystem::path>& filesToCompile);

    bool LinkModule(ModuleManifest* module, std::vector<std::filesystem::path>& sourceFiles);

    struct ModuleOrderInfo
    {
        ModuleManifest* module = nullptr;
        std::set<ModuleManifest*> dependencies;
        int orderIndex = 0;
    };
    int GetModuleOrderIndex(ModuleOrderInfo& orderInfo, std::map<ModuleManifest*, ModuleOrderInfo>& orderMap);

    std::string buildPlatform;
    std::string buildArchitecture;
    BuildType buildType;

    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
};
