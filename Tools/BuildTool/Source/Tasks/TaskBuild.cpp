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

    if (runInfo.projectManifest->modulesPaths.empty())
    {
        Utility::PrintLine("Project contains no modules");
        return false;
    }

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
        if (!runInfo.moduleList.empty())
        {
            auto it = std::find(runInfo.moduleList.begin(), runInfo.moduleList.end(), moduleInfo.module->name);
            if (it == runInfo.moduleList.end()) continue;
        }

        moduleInfo.module->fileDates.SetBuildType(runInfo.buildType);

        if (!CheckModule(moduleInfo)) return false;
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

bool TaskBuild::CheckModule(ModuleOrderInfo& moduleInfo)
{
    ModuleManifest* module = moduleInfo.module;

    if (module->sourceFiles.empty())
    {
        Utility::PrintLine("Module \"" + module->name + "\" contains no source files");
        return false;
    }

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

    if (!changeDetected)
    {
        std::filesystem::path binary = targetToolchain->GetBinaryPath(*module, targetPlatform, buildType);
        
        if (!std::filesystem::exists(binary))
        {
            Utility::PrintLineD("Binary does not exist");
            changeDetected = true;
        }
    }

    if (!changeDetected)
    {
        std::set<ModuleManifest*> dependencies = module->GetDependencyTree();
        for (ModuleManifest* dependency : dependencies)
        {
            std::filesystem::path binary = targetToolchain->GetBinaryPath(*dependency, targetPlatform, buildType);
            if ( timeDatabase.HasFileChanged(binary))
            {
                Utility::PrintLineD("Dependency binary has changed: " + dependency->name);
                changeDetected = true;
            }
        }
    }

    // Build module if something changed
    if (changeDetected)
    {
        Utility::PrintLine("Parsing include tree...");
        FileTree& fileTree = module->fileTree;
        fileTree.Parse();

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

    return true;
}

bool TaskBuild::BuildModule(ModuleManifest* module)
{
    FileTimeDatabase& timeDatabase = module->fileDates;
    FileTree& includeTree = module->fileTree;

    PerformanceTimer moduleTimer;

    std::vector<std::filesystem::path> source = module->sourceFiles;

    if (source.empty())
    {
        Utility::PrintLine("No source found");
        return false;
    }

    std::vector<std::filesystem::path> sourceFiles;
    std::vector<std::filesystem::path> filesToCompile = FindFilesToCompile(module, sourceFiles);

    timeDatabase.ClearDatabase();
    for (std::filesystem::path& file : source)
    {
        timeDatabase.SetFileTime(file);

        for (std::filesystem::path include : includeTree.GetTree(file))
        {
            timeDatabase.SetFileTime(include);
        }
    }

    std::set<ModuleManifest*> dependencies = module->GetDependencyTree();
    for (ModuleManifest* dependency : dependencies)
    {
        std::filesystem::path binary = targetToolchain->GetBinaryPath(*dependency, targetPlatform, buildType);
        if (timeDatabase.HasFileChanged(binary))
        {
            timeDatabase.SetFileTime(binary);
        }
    }

    PerformanceTimer prepareTimer;
    if (!targetToolchain->PrepareModuleForBuild(*module, targetPlatform, buildType)) return false;
    prepareTimer.Stop("Prepare");

    if (!filesToCompile.empty())
    {
        if (!CompileModule(module, filesToCompile)) return false;
    }

    if (!LinkModule(module, sourceFiles)) return false;

    module->fileDates.SaveDatabase();

    Utility::Print("\n");
    moduleTimer.Stop("Module: " + module->name);

    return true;
}

bool TaskBuild::CompileModule(ModuleManifest* module, std::vector<std::filesystem::path>& filesToCompile)
{
    PerformanceTimer buildTimer;

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

std::vector<std::filesystem::path> TaskBuild::FindFilesToCompile(ModuleManifest* module, std::vector<std::filesystem::path>& outSourceFiles)
{
    FileTimeDatabase& timeDatabase = module->fileDates;
    FileTree& includeTree = module->fileTree;

    std::vector<std::filesystem::path> filesToCompile;

    for (std::filesystem::path& file : module->sourceFiles)
    {
        std::string extension = file.extension().string();
        if (extension == ".c" || extension == ".cpp")
        {
            outSourceFiles.push_back(file);

            std::filesystem::path objPath = targetToolchain->GetObjPath(*module, targetPlatform, buildType, file);

            bool changed = timeDatabase.HasFileChanged(file);
            if (!changed)
            {
                for (std::filesystem::path include : includeTree.GetTree(file))
                {
                    if (timeDatabase.HasFileChanged(include))
                    {
                        changed = true;
                        break;
                    }
                }
            }

            if (!std::filesystem::exists(objPath) || changed)
            {
                filesToCompile.push_back(file);
            }
        }
    }

    return filesToCompile;
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
