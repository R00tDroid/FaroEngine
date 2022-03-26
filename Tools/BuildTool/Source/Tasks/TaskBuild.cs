using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using FaroEngine;

public class TaskBuild : ITask
{
    private string buildPlatform;
    private string buildArchitecture;
    public TaskBuild(string platform, string architecture)
    {
        buildPlatform = platform;
        buildArchitecture = architecture;
    }

    public override int GetPriority()
    {
        return 2;
    }

    IToolchain targetToolchain = null;
    BuildPlatform targetPlatform = null;

    public override bool Run(ProjectManifest project)
    {
        targetToolchain = null;
        targetPlatform = null;

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

            if (targetToolchain != null) break;
        }

        if (targetToolchain == null || targetPlatform == null) 
        {
            Utility.PrintLine("Unable to find suitable toolchain for platform: " + buildPlatform + " " + buildArchitecture);
            return false;
        }

        //TODO sort module order based on dependencies
        List<ModuleManifest> moduleOrder = new List<ModuleManifest>();
        moduleOrder = project.projectModules;

        Utility.PrintLine("Performing build...");

        foreach (var module in moduleOrder)
        {
            PerformanceTimer moduleTimer = new PerformanceTimer();

            List<string> source = module.sourceFiles;

            List<string> filesToCompile = new List<string>();
            List<string> sourceFiles = new List<string>();

     
            PerformanceTimer treescanTimer = new PerformanceTimer();
            foreach (var file in source)
            {
                string extension = Path.GetExtension(file);
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

                List<string> includes = new List<string>();
                includes.Add(module.sourceRoot);

                PerformanceTimer sourceFilesTimer = new PerformanceTimer();
                foreach (string file in filesToCompile) 
                {
                    PerformanceTimer fileTimer = new PerformanceTimer();
                    string displayName = file.Replace(module.sourceRoot + "\\", "");
                    Utility.PrintLine(displayName);

                    if (!targetToolchain.BuildSource(file, includes, targetPlatform.preprocessorDefines)) 
                    {
                        Utility.PrintLine("Build error!");
                        return false;
                    }
                    fileTimer.Stop("Source: " + displayName);
                }
                sourceFilesTimer.Stop("Build source");

                Utility.PrintLine("Generating module");
                targetToolchain.LinkLibrary(sourceFiles);

                buildTimer.Stop("Build");
            }

            Utility.Print("\n");
            moduleTimer.Stop("Module: " + module.name);
        }

        //TODO Only link when needed
        Utility.PrintLine("Linking modules");
        targetToolchain.LinkExecutable(project, moduleOrder);

        return true;
    }
}