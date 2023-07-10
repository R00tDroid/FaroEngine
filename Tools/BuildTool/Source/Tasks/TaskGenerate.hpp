#pragma once
#include <filesystem>
#include <tinyxml2.h>
#include "ITask.hpp"
#include "MSVCInfo.hpp"
#include "Toolchains/IToolchain.hpp"

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

        virtual bool HasSourceFiles() { return false; }
        virtual std::vector<std::filesystem::path> GetSourceFiles() { return {}; }
        virtual std::vector<std::filesystem::path> GetIncludePaths() { return {}; }

        virtual std::string GetBuildCommand() = 0;
        virtual std::string GetRebuildCommand() = 0;
        virtual std::string GetCleanCommand() = 0;

        virtual std::filesystem::path GetOutputBinary(IToolchain*, BuildPlatform*, BuildType) { return {}; }

        virtual std::filesystem::path GetRootDirectory() { return {}; };
    };

    struct CustomCommandInfo : ProjectInfo
    {
        std::string GetBuildCommand() override { return buildCommand; }
        std::string GetRebuildCommand() override { return rebuildCommand; }
        std::string GetCleanCommand() override { return cleanCommand; }

        std::string buildCommand, rebuildCommand, cleanCommand;
    };

    struct ModuleInfo : ProjectInfo
    {
        std::string GetBuildCommand() override
        {
            std::filesystem::path faroBuildTool = Utility::GetExecutablePath();
            return faroBuildTool.string() + " -build -project " + module->project->manifestPath.string() + " -module " + module->name;
        }
        std::string GetRebuildCommand() override
        {
            std::filesystem::path faroBuildTool = Utility::GetExecutablePath();
            return faroBuildTool.string() + " -clean -build -project " + module->project->manifestPath.string() + " -module " + module->name;
        }
        std::string GetCleanCommand() override
        {
            std::filesystem::path faroBuildTool = Utility::GetExecutablePath();
            return faroBuildTool.string() + " -clean -project " + module->project->manifestPath.string() + " -module " + module->name;
        }

        bool HasSourceFiles() override { return true; }
        std::vector<std::filesystem::path> GetSourceFiles() override
        {
            std::vector<std::filesystem::path> files = module->sourceFiles;
            files.push_back(module->manifestPath);
            return files;
        }
        std::vector<std::filesystem::path> GetIncludePaths() override { return module->GetModuleIncludeDirectories(); }
        std::filesystem::path GetRootDirectory() override { return module->moduleRoot; }

        std::filesystem::path GetOutputBinary(IToolchain* toolchain, BuildPlatform* platform, BuildType type) override
        {
            if (module->type == MT_Executable)
            {
                return toolchain->GetExePath(*module, platform, type);
            }
            else if (module->type == MT_Library)
            {
                return toolchain->GetLibPath(*module, platform, type);
            }
            else
            {
                return {};
            }
        }

        ModuleManifest* module = nullptr;
    };

    int GetPriority() const override
    {
        return 1;
    }

    bool Run(ProjectManifest& project) override
    {
        auto msvc_versions = GetMSVCInstallations();
        if (msvc_versions.empty())
        {
            Utility::PrintLine("Failed to find a Visual Studio installation");
            return false;
        }
        else
        {
            VSPlatformVersion = msvc_versions[0].RedistVersion;
        }

        PerformanceTimer timer;

        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            PerformanceTimer moduleTimer;
            if (!moduleManifest->Parse()) return false;
            moduleTimer.Stop("Parse manifest: " + moduleManifest->name);
        }

        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            PerformanceTimer moduleTimer;
            if (!moduleManifest->ResolveDependencies()) return false;
            moduleManifest->Save();
            moduleTimer.Stop("Resolve dependencies: " + moduleManifest->name);
        }

        Utility::PrintLine("Performing solution generation...");

        std::filesystem::path faroBuildTool = Utility::GetExecutablePath();

        timer.Stop("Parse module manifests");
        timer = {};

        std::vector<ProjectInfo*> projectInfoList;
        CustomCommandInfo* commandInfo = new CustomCommandInfo();
        commandInfo->name = "Package";
        commandInfo->buildByDefault = false;
        commandInfo->buildCommand = faroBuildTool.string() + " -build -project " + project.manifestPath.string();
        commandInfo->cleanCommand = faroBuildTool.string() + " -clean -project " + project.manifestPath.string();
        commandInfo->rebuildCommand = faroBuildTool.string() + " -clean -build -project " + project.manifestPath.string();
        commandInfo->uuid = Utility::GetCachedUUID(project.faroRoot / "ProjectInfo" / (commandInfo->name + "Id.txt"));
        commandInfo->projectPath = project.faroRoot / "Project" / (commandInfo->name + ".vcxproj");
        commandInfo->solutionPath = "Project/Actions";
        projectInfoList.push_back(commandInfo);

        commandInfo = new CustomCommandInfo();
        commandInfo->name = "Generate";
        commandInfo->buildByDefault = false;
        commandInfo->buildCommand = faroBuildTool.string() + " -generate -project " + project.manifestPath.string();
        commandInfo->rebuildCommand = faroBuildTool.string() + " -generate -build -project " + project.manifestPath.string();
        commandInfo->uuid = Utility::GetCachedUUID(project.faroRoot / "ProjectInfo" / (commandInfo->name + "Id.txt"));
        commandInfo->projectPath = project.faroRoot / "Project" / (commandInfo->name + ".vcxproj");
        commandInfo->solutionPath = "Project/Actions";
        projectInfoList.push_back(commandInfo);

        timer.Stop("Generate action projects");
        timer = {};

        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            PerformanceTimer moduleTimer;

            ModuleInfo* moduleInfo = new ModuleInfo();
            moduleInfo->name = moduleManifest->name;
            moduleInfo->module = moduleManifest;
            moduleInfo->uuid = moduleManifest->uuid;
            moduleInfo->projectPath = moduleManifest->project->faroRoot / "Project" / (moduleManifest->name + ".vcxproj");
            moduleInfo->solutionPath = "Project/Modules";
            moduleInfo->buildByDefault = true;
            moduleInfo->debuggable = moduleManifest->type == MT_Executable;
            if (!moduleManifest->solutionLocation.empty())
            {
                moduleInfo->solutionPath /= moduleManifest->solutionLocation;
            }

            for (ModuleManifest* dependency : moduleManifest->moduleDependencies)
            {
                moduleInfo->dependencyUuids.push_back(dependency->uuid);
            }

            projectInfoList.push_back(moduleInfo);

            moduleTimer.Stop("Module: " + moduleManifest->name);
        }

        for (ProjectInfo* projectInfo : projectInfoList)
        {
            WriteProjectFile(*projectInfo);
            WriteProjectUserFile(*projectInfo);

            if (projectInfo->HasSourceFiles())
            {
                WriteFilterFile(*projectInfo);
            }
        }

        timer.Stop("Generate module projects");

        timer = {};
        WriteSolutionFile(project, projectInfoList);
        timer.Stop("generate solution file");

        return true;
    }

private:
    std::string VSPlatformVersion = "";

    std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };

    void WriteProjectFile(ProjectInfo& projectInfo)
    {
        Utility::EnsureDirectory(projectInfo.projectPath.parent_path());

        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();

        tinyxml2::XMLDocument doc(false);
        {
            tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
            projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
            projectElement->SetAttribute("DefaultTargets", "Build");
            projectElement->SetAttribute("ToolsVersion", "Current");
            doc.InsertEndChild(projectElement);

            {
                tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");
                projectElement->SetAttribute("Label", "ProjectConfigurations");

                for (IToolchain* toolchain : toolchains)
                {
                    std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
                    for (BuildPlatform* platform : platforms)
                    {
                        for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                        {
                            const char* buildTypeName = BuildTypeNames[buildTypeIndex];

                            tinyxml2::XMLElement* projectConfig = itemGroup->InsertNewChildElement("ProjectConfiguration");
                            projectConfig->SetAttribute("Include", (platform->platformName + " " + buildTypeName + "|Win32").c_str());

                            {
                                tinyxml2::XMLElement* configElement = projectConfig->InsertNewChildElement("Configuration");
                                configElement->SetText((platform->platformName + " " + buildTypeName).c_str());
                            }
                            {
                                tinyxml2::XMLElement* platformElement = projectConfig->InsertNewChildElement("Platform");
                                platformElement->SetText("Win32");
                            }
                        }
                    }
                }
            }

            
            {
                tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
                propertyGroup->SetAttribute("Label", "Globals");

                tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("ProjectGuid");
                element->SetText(projectInfo.uuid.c_str());

                element = propertyGroup->InsertNewChildElement("PlatformToolset");
                element->SetText(VSPlatformVersion.c_str());


                element = propertyGroup->InsertNewChildElement("TargetRuntime");
                element->SetText("Native");
            }

            {
                tinyxml2::XMLElement* importElement = projectElement->InsertNewChildElement("Import");
                importElement->SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
            }

            for( IToolchain* toolchain : toolchains)
            {
                std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
                for (BuildPlatform* platform : platforms)
                {
                    for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                    {
                        const char* buildTypeName = BuildTypeNames[buildTypeIndex];

                        tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
                        propertyGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + platform->platformName + " " + buildTypeName + "|Win32'").c_str());
                        propertyGroup->SetAttribute("Label", "Configuration");

                        {
                            tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("ConfigurationType");
                            element->SetText("Makefile");
                        }
                        {
                            tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("PlatformToolset");
                            element->SetText(VSPlatformVersion.c_str());
                        }
                    }
                }
            }

            {
                tinyxml2::XMLElement* element = projectElement->InsertNewChildElement("Import");
                element->SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");
            }

            {
                tinyxml2::XMLElement* element = projectElement->InsertNewChildElement("ImportGroup");
                element->SetAttribute("Label", "ExtensionTargets");
            }

            {
                tinyxml2::XMLElement* element = projectElement->InsertNewChildElement("PropertyGroup");
                element->SetAttribute("Label", "UserMacros");
            }

            for (IToolchain* toolchain : toolchains)
            {
                std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
                for (BuildPlatform* platform : platforms)
                {
                    for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                    {
                        const char* buildTypeName = BuildTypeNames[buildTypeIndex];

                        tinyxml2::XMLElement* importGroup = projectElement->InsertNewChildElement("ImportGroup");
                        importGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + platform->platformName + " " + buildTypeName + "|Win32'").c_str());
                        importGroup->SetAttribute("Label", "PropertySheets");

                        {
                            tinyxml2::XMLElement* element = importGroup->InsertNewChildElement("Import");
                            element->SetAttribute("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
                            element->SetAttribute("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
                            element->SetAttribute("Label", "LocalAppDataPlatform");
                        }

                        tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
                        propertyGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + platform->platformName + " " + buildTypeName + "|Win32'").c_str());
                        {
                            tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("IncludePath");

                            std::string includePaths = "$(VC_IncludePath);$(WindowsSDK_IncludePath)"; //TODO directly reference used platform includes from toolchain
                            for (std::filesystem::path& path : projectInfo.GetIncludePaths())
                            {
                                includePaths += ";" + path.string();
                            }
                            includePaths += ";";

                            element->SetText(includePaths.c_str());

                            element = propertyGroup->InsertNewChildElement("ReferencePath");
                            element = propertyGroup->InsertNewChildElement("LibraryPath");
                            element = propertyGroup->InsertNewChildElement("LibraryWPath");
                            element = propertyGroup->InsertNewChildElement("SourcePath");
                            element = propertyGroup->InsertNewChildElement("ExcludePath");

                            std::filesystem::path faroBuildTool = Utility::GetExecutablePath();

                            element = propertyGroup->InsertNewChildElement("OutDir");
                            element->SetText("$(ProjectDir)\\..\\Bin");

                            element = propertyGroup->InsertNewChildElement("IntDir");
                            element->SetText("$(ProjectDir)\\Intermediate");

                            element = propertyGroup->InsertNewChildElement("NMakeBuildCommandLine");
                            element->SetText((projectInfo.GetBuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
                            element->SetText((projectInfo.GetRebuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
                            element->SetText((projectInfo.GetCleanCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeOutput");

                            std::filesystem::path outputPath = projectInfo.GetOutputBinary(toolchain, platform, (BuildType)buildTypeIndex);
                            if (!outputPath.empty())
                            {
                                element->SetText(outputPath.string().c_str());
                            }
                         
                            element = propertyGroup->InsertNewChildElement("NMakeIncludeSearchPath");
                            element->SetText(includePaths.c_str());

                            element = propertyGroup->InsertNewChildElement("NMakePreprocessorDefinitions");

                            std::string preprocessorDefines;
                            for (std::string& path : toolchain->GetPreprocessorDefines(platform, static_cast<BuildType>(buildTypeIndex)))
                            {
                                preprocessorDefines += path + ";";
                            }

                            element->SetText(preprocessorDefines.c_str());
                        }
                    }
                }
            }

            {
                tinyxml2::XMLElement* importElement = projectElement->InsertNewChildElement("Import");
                importElement->SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.targets");
            }

            {
                tinyxml2::XMLElement* ImportGroupElement = projectElement->InsertNewChildElement("ImportGroup");
                ImportGroupElement->SetAttribute("Label", "ExtensionTargets");
            }

            {
                tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");
                std::vector<std::filesystem::path> sourceFiles = projectInfo.GetSourceFiles();
                for (std::filesystem::path& file : sourceFiles)
                {
                    std::string extension = file.extension().string();
                    extension = Utility::ToLower(extension);
                    bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();
                    tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
                    fileElement->SetAttribute("Include", file.string().c_str());
                }
            }
        }

        doc.SaveFile(projectInfo.projectPath.string().c_str());
    }

    void WriteProjectUserFile(ProjectInfo& projectInfo)
    {
        if (!projectInfo.debuggable) return;

        std::filesystem::path file = projectInfo.projectPath;
        file.replace_extension(".vcxproj.user");

        Utility::EnsureDirectory(file.parent_path());

        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();

        tinyxml2::XMLDocument doc(false);
        {
            tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
            projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
            projectElement->SetAttribute("ToolsVersion", "Current");
            doc.InsertEndChild(projectElement);

            
            for (IToolchain* toolchain : toolchains)
            {
                std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
                for (BuildPlatform* platform : platforms)
                {
                    for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                    {
                        const char* buildTypeName = BuildTypeNames[buildTypeIndex];

                        tinyxml2::XMLElement* propGroup = projectElement->InsertNewChildElement("PropertyGroup");
                        propGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)'=='" + platform->platformName + " " + buildTypeName + "|Win32'").c_str());

                        tinyxml2::XMLElement* element = propGroup->InsertNewChildElement("LocalDebuggerCommand");
                        element->SetText(projectInfo.GetOutputBinary(toolchain, platform, (BuildType)buildTypeIndex).string().c_str());
                        element = propGroup->InsertNewChildElement("DebuggerFlavor");
                        element->SetText("WindowsLocalDebugger");
                    }
                }
            }
        }
        

        doc.SaveFile(file.string().c_str());
    }

    static std::filesystem::path GetFileRelativeDirectory(std::filesystem::path root, std::filesystem::path file)
    {
        std::filesystem::path directory = std::filesystem::absolute(file.parent_path());
        return std::filesystem::proximate(directory, root);
    }

    static std::vector<std::filesystem::path> GetDirectoryTree(std::filesystem::path& root)
    {
        std::vector<std::filesystem::path> result = { root };
        while (true)
        {
            std::filesystem::path& current = result[result.size() - 1];
            std::filesystem::path parent = current.parent_path();
            if (!parent.empty())
            {
                result.push_back(parent);
            }
            else
            {
                break;
            }
        }

        return result;
    }

    void WriteFilterFile(ProjectInfo& projectInfo)
    {
        std::filesystem::path filePath = projectInfo.projectPath;
        filePath.replace_extension(".vcxproj.filters");

        tinyxml2::XMLDocument doc(false);

        tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
        projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
        projectElement->SetAttribute("ToolsVersion", "Current");
        doc.InsertEndChild(projectElement);

        tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");

        std::vector<std::filesystem::path> sourceFiles = projectInfo.GetSourceFiles();

        std::map<std::filesystem::path, std::string> directories;
        for (std::filesystem::path& file : sourceFiles)
        {
            std::filesystem::path directory = GetFileRelativeDirectory(projectInfo.GetRootDirectory(), file);
            if (!directory.empty() && directory.string() != ".") 
            {
                std::vector<std::filesystem::path> allDirectories = GetDirectoryTree(directory);

                for (std::filesystem::path& dir : allDirectories)
                {
                    if (directories.find(dir) == directories.end())
                    {
                        directories.insert(std::pair<std::filesystem::path, std::string>(dir, Utility::GenerateUUID()));
                    }
                }

                std::string extension = file.extension().string();
                extension = Utility::ToLower(extension);
                bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();

                tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
                fileElement->SetAttribute("Include", file.string().c_str());

                tinyxml2::XMLElement* filterElement = fileElement->InsertNewChildElement("Filter");
                filterElement->SetText(directory.string().c_str());
            }
        }

        for (auto& directory : directories)
        {
            tinyxml2::XMLElement* filterElement = itemGroup->InsertNewChildElement("Filter");
            filterElement->SetAttribute("Include", directory.first.string().c_str());

            tinyxml2::XMLElement* idElement = filterElement->InsertNewChildElement("UniqueIdentifier");
            idElement->SetText(("{" + directory.second + "}").c_str());
        }

        doc.SaveFile(filePath.string().c_str());
    }

    void WriteSolutionFile(ProjectManifest project, std::vector<ProjectInfo*> projectInfoList)
    {
        std::ofstream stream(project.projectDirectory / (project.projectName + ".sln"));

        stream << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        stream << "# Visual Studio 16\n";
        stream << "MinimumVisualStudioVersion = 10.0.40219.1\n";

        for (ProjectInfo* projectInfo : projectInfoList)
        {
            stream << "Project(\"{" + project.uuid + "}\") = \"" + projectInfo->name + "\", \"" + (projectInfo->projectPath).string() + "\", \"{" + projectInfo->uuid + "}\"\n";
            if (!projectInfo->dependencyUuids.empty())
            {
                stream << "\tProjectSection(ProjectDependencies) = postProject\n";
                for (std::string& dependency : projectInfo->dependencyUuids)
                {
                    stream << "\t\t{" << dependency << "} = {" << dependency << "}\n";
                }
                stream << "\tEndProjectSection\n";
            }
            stream << "EndProject\n";
        }

        std::map<std::filesystem::path, std::string> solutionDirectories;
        for (ProjectInfo* projectInfo : projectInfoList)
        {
            projectInfo->solutionPath.make_preferred();
            projectInfo->solutionPath = std::filesystem::weakly_canonical(projectInfo->solutionPath);

            std::vector<std::filesystem::path> tree = GetDirectoryTree(projectInfo->solutionPath);
            for (std::filesystem::path& solutionDir : tree)
            {
                auto it = solutionDirectories.find(solutionDir);
                if (it == solutionDirectories.end())
                {
                    solutionDirectories.insert(std::pair<std::filesystem::path, std::string>(solutionDir, Utility::GetCachedUUID(project.faroRoot / "ProjectInfo" / ("Dir" + solutionDir.filename().string() + "Id.txt"))));
                }
            }
        }

        for (auto& it : solutionDirectories)
        {
            stream << "Project(\"{2150E333-8FDC-42A3-9474-1A3956D46DE8}\") = \"" + it.first.filename().string() + "\", \"" + it.first.filename().string() + "\", \"{" + it.second + "}\"\n";
            stream << "EndProject\n";
        }

        stream << "Global\n";
        stream << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();
        for (IToolchain* toolchain : toolchains)
        {
            std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
            for (BuildPlatform* platform : platforms)
            {
                for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                {
                    const char* buildTypeName = BuildTypeNames[buildTypeIndex];
                    stream << "\t\t" << platform->platformName << " " << buildTypeName << "|x86 = " << platform->platformName << " " << buildTypeName << "|x86\n";
                }
            }
        }
        stream << "\tEndGlobalSection\n";

        stream << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
        for (ProjectInfo* projectInfo : projectInfoList)
        {
            WriteSolutionProjectConfig(stream, *projectInfo);
        }
        stream << "\tEndGlobalSection\n";

        stream << "\tGlobalSection(NestedProjects) = preSolution\n";
        for (ProjectInfo* projectInfo : projectInfoList)
        {
            stream << "\t\t{" << projectInfo->uuid << "} = {" << solutionDirectories[projectInfo->solutionPath] << "}\n";
        }
        for (auto& it : solutionDirectories)
        {
            std::filesystem::path parent = it.first.parent_path();
            if (!parent.empty())
            {
                stream << "\t\t{" << it.second << "} = {" << solutionDirectories[parent] << "}\n";
            }
        }
        stream << "\tEndGlobalSection\n";

        stream << "EndGlobal\n";

        stream.close();
    }

    inline static  void WriteSolutionProjectConfig(std::ofstream& stream, ProjectInfo& projectInfo)
    {
        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();
        for (IToolchain* toolchain : toolchains)
        {
            std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
            for (BuildPlatform* platform : platforms)
            {
                for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
                {
                    const char* buildTypeName = BuildTypeNames[buildTypeIndex];
                    stream << "\t\t{" << projectInfo.uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.ActiveCfg = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                    if (projectInfo.buildByDefault)
                    {
                        stream << "\t\t{" << projectInfo.uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.Build.0 = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                    }
                }
            }
        }
    }
};
