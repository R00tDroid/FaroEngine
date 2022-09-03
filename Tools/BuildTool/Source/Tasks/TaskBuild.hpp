#pragma once
#include <string>

#include "ITask.hpp"
#include "../Utility.hpp"


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
        return filePath.lexically_proximate(module.moduleRoot).string();
    }

    bool Run(ProjectManifest& project) override
    {
        targetToolchain = nullptr;
        targetPlatform = nullptr;

        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();
        for (IToolchain* toolchain : toolchains) 
        {
            std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
            for (BuildPlatform* platform : platforms)
            {
                std::string platformName = platform->platformName;
                std::string requiredPlatform = buildPlatform + " " + buildArchitecture;

                std::transform(platformName.begin(), platformName.end(), platformName.begin(), tolower);
                std::transform(requiredPlatform.begin(), requiredPlatform.end(), requiredPlatform.begin(), tolower);

                if (platformName == requiredPlatform)
                {
                    targetToolchain = toolchain;
                    targetPlatform = platform;
                    break;
                }
            }

            if (targetToolchain != nullptr) break;
        }

        if (targetToolchain == nullptr || targetPlatform == nullptr) 
        {
            Utility::PrintLine("Unable to find suitable toolchain for platform: " + buildPlatform + " " + buildArchitecture);
            return false;
        }

        //TODO sort module order based on dependencies
        std::vector<ModuleManifest*> moduleOrder = project.projectModules;

        Utility::PrintLine("Performing build...");

        for (ModuleManifest* module : moduleOrder)
        {
            PerformanceTimer moduleTimer;

            Utility::PrintLine("Module: " + module->name);

            std::vector<std::filesystem::path> source = module->sourceFiles;

            if (source.empty())
            {
                Utility::PrintLine("No source found");
                return false;
            }

            std::vector<std::filesystem::path> filesToCompile;
            std::vector<std::filesystem::path> sourceFiles;

     
            PerformanceTimer treescanTimer;
            for (std::filesystem::path& file : source)
            {
                std::string extension = file.extension().string();
                if (extension == ".c" || extension == ".cpp")
                {
                    sourceFiles.push_back(file);

                    //TODO check for changes
                    filesToCompile.push_back(file);
                    
                }
            }
            treescanTimer.Stop("Change check treescan");

            if (filesToCompile.empty())
            {
                Utility::PrintLine("All files up-to-date");
            }
            {
                PerformanceTimer buildTimer;

                PerformanceTimer prepareTimer;
                if (!targetToolchain->PrepareModuleForBuild(*module, targetPlatform, buildType)) return false;
                prepareTimer.Stop("Prepare");

                std::vector<std::filesystem::path> includes;// = module->GetPublicIncludeTree();
                //includes = includes.Concat(module->privateIncludeDirectories).ToList();
                //includes.Sort();

                PerformanceTimer sourceFilesTimer;
                for (std::filesystem::path& file : filesToCompile)
                {
                    PerformanceTimer fileTimer;
                    std::string displayName = GetDisplayName(*module, file);
                    Utility::PrintLine(displayName);

                    if (!targetToolchain->BuildSource(*module, targetPlatform, buildType, file, includes, targetPlatform->preprocessorDefines)) 
                    {
                        Utility::PrintLine("Build error!");
                        return false;
                    }
                    fileTimer.Stop("Source: " + displayName);
                }
                sourceFilesTimer.Stop("Build source");

                Utility::PrintLine("Generating library...");
                if (!targetToolchain->LinkLibrary(*module, targetPlatform, buildType, sourceFiles))
                {
                    Utility::PrintLine("Linking error!");
                    return false;
                }

                //TODO link all modules into executable

                buildTimer.Stop("Build");
            }

            Utility::Print("\n");
            moduleTimer.Stop("Module: " + module->name);
        }

        //TODO Only link when needed
        //Utility::PrintLine("Linking modules");
        //targetToolchain.LinkExecutable(module, targetPlatform, project, moduleOrder);

        return true;
    }

private:
    std::string buildPlatform;
    std::string buildArchitecture;
    BuildType buildType;

    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
};
