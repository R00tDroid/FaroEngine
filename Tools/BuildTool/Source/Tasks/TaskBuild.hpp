#pragma once
#include <string>

#include "ITask.hpp"
#include "../Utility.hpp"


class TaskBuild : public ITask
{
public:
    TaskBuild(std::string platform, std::string architecture)
    {
        buildPlatform = platform;
        buildArchitecture = architecture;
    }

    int GetPriority() override
    {
        return 2;
    }



    std::string GetDisplayName(ModuleManifest module, std::string filePath)
    {
        var origin = new Uri(module.moduleRoot);
        var relative = Uri.UnescapeDataString(origin.MakeRelativeUri(new Uri(filePath)).ToString()).Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
        return relative;
    }

    bool Run(ProjectManifest& project) override
    {
        targetToolchain = nullptr;
        targetPlatform = nullptr;

        List<IToolchain> toolchains = IToolchain.GetToolchains();
        foreach (IToolchain toolchain in toolchains) 
        {
            List<BuildPlatform> platforms = toolchain.GetPlatforms();
            foreach (BuildPlatform platform in platforms)
            {
                if (platform.platformName.ToLower() == (buildPlatform + " " + buildArchitecture).ToLower()) 
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
        List<ModuleManifest> moduleOrder = new List<ModuleManifest>();
        moduleOrder = project.projectModules;

        Utility::PrintLine("Performing build...");

        foreach (var module in moduleOrder)
        {
            PerformanceTimer moduleTimer = new PerformanceTimer();

            List<std::string> source = module.sourceFiles;

            List<std::string> filesToCompile = new List<std::string>();
            List<std::string> sourceFiles = new List<std::string>();

     
            PerformanceTimer treescanTimer = new PerformanceTimer();
            foreach (var file in source)
            {
                std::string extension = Path.GetExtension(file);
                if (extension == ".c" || extension == ".cpp")
                {
                    sourceFiles.Add(file);

                    //TODO check for changes
                    filesToCompile.Add(file);
                    
                }
            }
            treescanTimer.Stop("Change check treescan");

            if (filesToCompile.Count > 0)
            {
                PerformanceTimer buildTimer = new PerformanceTimer();

                PerformanceTimer prepareTimer = new PerformanceTimer();
                if (!targetToolchain.PrepareModuleForBuild(module, targetPlatform)) return false;
                prepareTimer.Stop("Prepare");

                List<std::string> includes = module.GetPublicIncludeTree();
                includes = includes.Concat(module.privateIncludeDirectories).ToList();
                includes.Sort();

                PerformanceTimer sourceFilesTimer = new PerformanceTimer();
                foreach (std::string file in filesToCompile) 
                {
                    PerformanceTimer fileTimer = new PerformanceTimer();
                    std::string displayName = GetDisplayName(module, file);
                    Utility::PrintLine(displayName);

                    if (!targetToolchain.BuildSource(module, targetPlatform, file, includes, targetPlatform.preprocessorDefines)) 
                    {
                        Utility::PrintLine("Build error!");
                        return false;
                    }
                    fileTimer.Stop("Source: " + displayName);
                }
                sourceFilesTimer.Stop("Build source");

                switch (module.GetModuleType())
                {
                    case ModuleType.Library:
                    {
                        Utility::PrintLine("Generating library...");
                        targetToolchain.LinkLibrary(module, targetPlatform, sourceFiles);
                        break;
                    }

                    case ModuleType.Executable:
                    {
                        Utility::PrintLine("Generating executable...");
                        targetToolchain.LinkExecutable(module, targetPlatform, sourceFiles);
                        break;
                    }

                    default:
                    {
                        Utility::PrintLine("Unknown module type");
                        return false;
                    }
                }

                buildTimer.Stop("Build");
            }

            Utility::Print("\n");
            moduleTimer.Stop("Module: " + module.name);
        }

        //TODO Only link when needed
        //Utility::PrintLine("Linking modules");
        //targetToolchain.LinkExecutable(module, targetPlatform, project, moduleOrder);

        return true;
    }

private:
    std::string buildPlatform;
    std::string buildArchitecture;

    IToolchain* targetToolchain = nullptr;
    BuildPlatform* targetPlatform = nullptr;
}F