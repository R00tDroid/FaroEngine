#include "TaskBuild.hpp"
#include <Utility.hpp>

TaskBuild::TaskBuild(std::string platform, std::string architecture, BuildType configuration)
{
    buildPlatform = platform;
    buildArchitecture = architecture;
    buildType = configuration;
}

int TaskBuild::GetPriority() const
{
    return 3;
}

std::string TaskBuild::GetDisplayName(ModuleManifest& module, std::filesystem::path& filePath)
{
    return filePath.lexically_proximate(module.manifestDirectory).string();
}

bool TaskBuild::Run(TaskRunInfo& runInfo)
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

    std::map<ModuleManifest*, ModuleOrderInfo> moduleOrderInfo;

    for (std::filesystem::path& modulePath : runInfo.projectManifest->modulesPaths)
    {
        ModuleManifest* module = ModuleManifest::GetLoadedModule(modulePath);
        moduleOrderInfo.insert(std::pair<ModuleManifest*, ModuleOrderInfo>(module, { module, module->GetDependencies() }));
    }

    for (auto& it : moduleOrderInfo)
    {
        it.second.orderIndex = GetModuleOrderIndex(it.second, moduleOrderInfo);
    }

    std::vector<ModuleOrderInfo> moduleOrder;
    for (auto& it : moduleOrderInfo)
    {
        moduleOrder.push_back(it.second);
    }

    std::sort(moduleOrder.begin(), moduleOrder.end(), [](const ModuleOrderInfo& a, const ModuleOrderInfo& b)
    {
        return a.orderIndex < b.orderIndex;
    });

    for (ModuleOrderInfo& moduleInfo : moduleOrder)
    {
        ModuleManifest* module = moduleInfo.module;
        Utility::PrintLine("[" + module->name + "]");
        Utility::PrintLine("Checking for changes...");

        FileTimeDatabase& timeDatabase = module->fileDates;
        timeDatabase.LoadDatabase();
        timeDatabase.CheckForChanges();

        bool changeDetected = timeDatabase.HasAnyFileChanged();

#ifndef NDEBUG
        for (std::filesystem::path& file : timeDatabase.GetChangedFiles())
        {
            Utility::PrintLineD("Changed in DB: " + file.string());
        }
#endif

        // Check all source files. Perhaps there's a new file that's not in the database yet!
        if (!changeDetected)
        {
            for (std::filesystem::path& file : module->sourceFiles)
            {
                if (timeDatabase.HasFileChanged(file))
                {
                    changeDetected = true;

#ifndef NDEBUG
                    Utility::PrintLineD("Changed in source: " + file.string());
#else
                    break;
#endif
                }
            }
        }

        // Build module if something changed
        if (changeDetected)
        {
            Utility::PrintLine("Performing build...");
            if (!BuildModule(module))
            {
                return false;
            }
        }
        else
        {
            Utility::PrintLine("Everything is up-to-date");
        }
    }
    return true;
}

bool TaskBuild::FindToolchain()
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

bool TaskBuild::BuildModule(ModuleManifest* module)
{
    FileTimeDatabase& timeDatabase = module->fileDates;

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

            if (!std::filesystem::exists(objPath) || timeDatabase.HasFileChanged(file)) //TODO Check tree instead of database directly
            {
                filesToCompile.push_back(file);
            }
        }
    }

    timeDatabase.ClearDatabase();
    for (std::filesystem::path& file : source)
    {
        timeDatabase.SetFileTime(file); //TODO Add all files. Not just this module but the entire include tree!
    }

    if (filesToCompile.empty())
    {
        Utility::PrintLine("All files up-to-date");
    }
    else
    {
        if (!CompileModule(module, filesToCompile)) return false;
        if (!LinkModule(module, sourceFiles)) return false;
    }

    module->fileDates.SaveDatabase();

    Utility::Print("\n");
    moduleTimer.Stop("Module: " + module->name);

    return true;
}

bool TaskBuild::CompileModule(ModuleManifest* module, std::vector<std::filesystem::path>& filesToCompile)
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

        if (!targetToolchain->BuildSource(*module, targetPlatform, buildType, file, includes, targetToolchain->GetPreprocessorDefines(targetPlatform, buildType)))
        {
            anyError = true;
            module->fileDates.InvalidateFileTime(file);
        }
        fileTimer.Stop("Source: " + displayName);
    }

    if (anyError)
    {
        Utility::PrintLine("Build error!");
        return false;
    }
    sourceFilesTimer.Stop("Build source");

    buildTimer.Stop("Build");

    return true;
}

bool TaskBuild::LinkModule(ModuleManifest* module, std::vector<std::filesystem::path>& sourceFiles)
{
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

    return true;
}

int TaskBuild::GetModuleOrderIndex(ModuleOrderInfo& orderInfo, std::map<ModuleManifest*, ModuleOrderInfo>& orderMap)
{
    int index = 0;
    for (ModuleManifest* dependency : orderInfo.dependencies)
    {
        int dependencyOrder = GetModuleOrderIndex(orderMap[dependency], orderMap);
        index = std::max(index, dependencyOrder + 1);
    }

    return index;
}
