#pragma once
#include <string>
#include "ITask.hpp"
#include <Utility.hpp>

class TaskBuild : public ITask
{
public:
    TaskBuild(std::string platform, std::string architecture, BuildType configuration)
    {
        buildPlatform = platform;
        buildArchitecture = architecture;
        buildType = configuration;
    }

    int GetPriority() const override
    {
        return 3;
    }

    static std::string GetDisplayName(ModuleManifest& module, std::filesystem::path& filePath)
    {
        return filePath.lexically_proximate(module.manifestDirectory).string();
    }

    bool Run(TaskRunInfo& runInfo) override
    {
        if (runInfo.projectManifest == nullptr)
        {
            runInfo.projectManifest = ProjectManifest::LoadFromCache(runInfo.projectManifestPath);
        }

        targetToolchain = nullptr;
        targetPlatform = nullptr;

        if (!FindToolchain() || targetToolchain == nullptr || targetPlatform == nullptr) 
        {
            Utility::PrintLine("Unable to find suitable toolchain for platform: " + buildPlatform + " " + buildArchitecture);
            return false;
        }

        //TODO sort module order based on dependencies
        std::vector<std::filesystem::path> moduleOrder = runInfo.projectManifest->modulesPaths;

        for (std::filesystem::path& modulePath : moduleOrder)
        {
            ModuleManifest* module = ModuleManifest::GetLoadedModule(modulePath);
            Utility::PrintLine("[" + module->name + "]");
            Utility::PrintLine("Checking for changes...");
            //TODO Check for changes
            //Utility::PrintLine("Everything is up-to-date");

            Utility::PrintLine("Performing build...");
            if (!BuildModule(module))
            {
                return false;
            }
        }
        return true;
    }

private:
    bool FindToolchain()
    {
        PerformanceTimer toolchainTimer;
        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();
        for (IToolchain* toolchain : toolchains)
        {
            std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
            for (BuildPlatform* platform : platforms)
            {
                std::string platformName = platform->platformName;
                std::string requiredPlatform = buildPlatform + " " + buildArchitecture;

                platformName = Utility::ToLower(platformName);
                requiredPlatform = Utility::ToLower(requiredPlatform);

                if (platformName == requiredPlatform)
                {
                    targetToolchain = toolchain;
                    targetPlatform = platform;

                    toolchainTimer.Stop("Find toolchain");
                    return true;
                }
            }
        }

        return false;
    }

    bool BuildModule(ModuleManifest* module)
    {
        PerformanceTimer moduleTimer;

        std::vector<std::filesystem::path> source = module->sourceFiles;

        if (source.empty())
        {
            Utility::PrintLine("No source found");
            return false;
        }

        std::vector<std::filesystem::path> filesToCompile;
        std::vector<std::filesystem::path> sourceFiles;

        for (std::filesystem::path& file : source)
        {
            std::string extension = file.extension().string();
            if (extension == ".c" || extension == ".cpp")
            {
                sourceFiles.push_back(file);

                std::filesystem::path objPath = targetToolchain->GetObjPath(*module, targetPlatform, buildType, file);

                if (!std::filesystem::exists(objPath) || module->fileDates.HasTreeChanged(file))
                {
                    filesToCompile.push_back(file);
                }
                else
                {
                    module->fileDates.MarkTreeUpdate(file);
                }
            }
        }

        if (filesToCompile.empty())
        {
            Utility::PrintLine("All files up-to-date");
        }
        else
        {
            if (!CompileAndLink(module, sourceFiles, filesToCompile)) return false;
        }

        Utility::Print("\n");
        moduleTimer.Stop("Module: " + module->name);

        return true;
    }

    bool CompileAndLink(ModuleManifest* module, std::vector<std::filesystem::path>& sourceFiles, std::vector<std::filesystem::path>& filesToCompile)
    {
        PerformanceTimer buildTimer;

        PerformanceTimer prepareTimer;
        if (!targetToolchain->PrepareModuleForBuild(*module, targetPlatform, buildType)) return false;
        prepareTimer.Stop("Prepare");

        std::vector<std::filesystem::path> includes = module->GetModuleIncludeDirectories();

        PerformanceTimer sourceFilesTimer;
        bool anyError = false;
        for (std::filesystem::path& file : filesToCompile)
        {
            PerformanceTimer fileTimer;
            std::string displayName = GetDisplayName(*module, file);
            Utility::PrintLine(displayName);

            if (targetToolchain->BuildSource(*module, targetPlatform, buildType, file, includes, targetToolchain->GetPreprocessorDefines(targetPlatform, buildType)))
            {
                module->fileDates.MarkTreeUpdate(file);
            }
            else
            {
                anyError = true;
                module->fileDates.MarkFileInvalid(file);
            }
            fileTimer.Stop("Source: " + displayName);
        }

        module->fileDates.Save();

        if (anyError)
        {
            Utility::PrintLine("Build error!");
            return false;
        }
        sourceFilesTimer.Stop("Build source");

        PerformanceTimer linkTimer;
        switch (module->type)
        {
            case MT_Library: 
            {
                Utility::PrintLine("Generating library...");
                if (!targetToolchain->LinkLibrary(*module, targetPlatform, buildType, sourceFiles))
                {
                    Utility::PrintLine("Linking error!");
                    return false;
                }
                break;
            }
            case MT_Executable:
            {
                Utility::PrintLine("Generating executable...");
                if (!targetToolchain->LinkExecutable(*module, targetPlatform, buildType, sourceFiles))
                {
                    Utility::PrintLine("Linking error!");
                    return false;
                }
                break;
            }
        }

        
        linkTimer.Stop("Link module");
        buildTimer.Stop("Build");

        return true;
    }

    std::string buildPlatform;
    std::string buildArchitecture;
    BuildType buildType;

    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
};
