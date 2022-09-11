#pragma once
#include <filesystem>
#include <tinyxml2.h>
#include "ITask.hpp"
#include "Toolchains/IToolchain.hpp"

class TaskGenerate : public ITask
{
public:
    struct CustomCommandInfo
    {
        std::string name;
        std::filesystem::path projectPath;
        std::string uuid;
        bool buildByDefault = false;
    };

    int GetPriority() const override
    {
        return 1;
    }

    bool Run(ProjectManifest& project) override
    {
        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            if (!moduleManifest->Parse()) return false;
        }

        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            if (!moduleManifest->ResolveDependencies()) return false;
            moduleManifest->Save();
        }

        Utility::PrintLine("Performing solution generation...");

        PerformanceTimer timer;
        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            PerformanceTimer moduleTimer;

            PerformanceTimer writeTimer;
            WriteProjectFile(*moduleManifest);
            writeTimer.Stop("Project file");

            writeTimer = {};
            WriteFilterFile(*moduleManifest);
            writeTimer.Stop("Filter file");

            moduleTimer.Stop("Module: " + moduleManifest->name);
        }
        timer.Stop("Generate module projects");

        std::filesystem::path faroBuildTool = Utility::GetExecutablePath();

        std::vector<CustomCommandInfo> customCommands;

        WriteCommandProjectFile(project, customCommands, true, "Build",
            faroBuildTool.string() + " -build -project " + project.manifestPath.string(), 
            faroBuildTool.string() + " -clean -project " + project.manifestPath.string(), 
            faroBuildTool.string() + " -clean -build -project " + project.manifestPath.string());

        WriteCommandProjectFile(project, customCommands, false, "Generate",
            faroBuildTool.string() + " -generate -project " + project.manifestPath.string(), 
            faroBuildTool.string() + " -generate -project " + project.manifestPath.string(),
            faroBuildTool.string() + " -generate -project " + project.manifestPath.string());

        timer = {};
        WriteSolutionFile(project, customCommands);
        timer.Stop("generate solution file");

        return true;
    }

private:
    std::string VSPlatformVersion = "v142";
    std::string VSVersion = "16.0";

    std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };

    void WriteProjectFile(ModuleManifest& moduleManifest)
    {
        std::filesystem::path filePath = moduleManifest.project->faroRoot / "Project";
        Utility::EnsureDirectory(filePath);
        filePath /= moduleManifest.name + ".vcxproj";

        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();

        tinyxml2::XMLDocument doc;
        {
            tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
            projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
            projectElement->SetAttribute("DefaultTargets", "Build");
            projectElement->SetAttribute("ToolsVersion", "15.0");
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
                element->SetText(moduleManifest.uuid.c_str());

                element = propertyGroup->InsertNewChildElement("PlatformToolset");
                element->SetText(VSPlatformVersion.c_str());

                element = propertyGroup->InsertNewChildElement("MinimumVisualStudioVersion");
                element->SetText(VSVersion.c_str());

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
                            for (std::filesystem::path& path : moduleManifest.GetModuleIncludeDirectories())
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
                            element->SetText((faroBuildTool.string() + " -build -project " + moduleManifest.project->manifestPath.string() + " -module " + moduleManifest.name + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
                            element->SetText((faroBuildTool.string() + " -clean -build -project " + moduleManifest.project->manifestPath.string() + " -module " + moduleManifest.name + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
                            element->SetText((faroBuildTool.string() + " -clean -project " + moduleManifest.project->manifestPath.string() + " -module " + moduleManifest.name + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeOutput");
                            element->SetText("");

                            element = propertyGroup->InsertNewChildElement("NMakeIncludeSearchPath");
                            element->SetText(includePaths.c_str());

                            element = propertyGroup->InsertNewChildElement("NMakePreprocessorDefinitions");
                            element->SetText(""); //TODO set preprocessor defines
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
                std::vector<std::filesystem::path>& sourceFiles = moduleManifest.sourceFiles;
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

        doc.SaveFile(filePath.string().c_str());
    }

    void WriteCommandProjectFile(ProjectManifest& project, std::vector<CustomCommandInfo>& customCommands, bool buildByDefault, std::string name, std::string buildCommand, std::string cleanCommand, std::string rebuildCommand)
    {
        CustomCommandInfo commandInfo;
        commandInfo.name = name;
        commandInfo.buildByDefault = buildByDefault;
        commandInfo.uuid = Utility::GetCachedUUID(project.faroRoot / "ProjectInfo" / (name + "Id.txt"));

        commandInfo.projectPath = project.faroRoot / "Project";
        Utility::EnsureDirectory(commandInfo.projectPath);
        commandInfo.projectPath /= name + ".vcxproj";

        customCommands.push_back(commandInfo);

        std::vector<IToolchain*> toolchains = IToolchain::GetToolchains();

        tinyxml2::XMLDocument doc;
        {
            tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
            projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
            projectElement->SetAttribute("DefaultTargets", "Build");
            projectElement->SetAttribute("ToolsVersion", "15.0");
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
                element->SetText(commandInfo.uuid.c_str());

                element = propertyGroup->InsertNewChildElement("PlatformToolset");
                element->SetText(VSPlatformVersion.c_str());

                element = propertyGroup->InsertNewChildElement("MinimumVisualStudioVersion");
                element->SetText(VSVersion.c_str());

                element = propertyGroup->InsertNewChildElement("TargetRuntime");
                element->SetText("Native");
            }

            {
                tinyxml2::XMLElement* importElement = projectElement->InsertNewChildElement("Import");
                importElement->SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
            }

            for (IToolchain* toolchain : toolchains)
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
                            element->SetText((buildCommand + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
                            element->SetText((rebuildCommand + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
                            element->SetText((cleanCommand + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

                            element = propertyGroup->InsertNewChildElement("NMakeOutput");
                            element->SetText("");

                            element = propertyGroup->InsertNewChildElement("NMakeIncludeSearchPath");
                            element->SetText("");

                            element = propertyGroup->InsertNewChildElement("NMakePreprocessorDefinitions");
                            element->SetText("");
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
        }

        doc.SaveFile(commandInfo.projectPath.string().c_str());
    }

    static std::filesystem::path GetFileRelativeDirectory(ModuleManifest& moduleManifest, std::filesystem::path file)
    {
        std::filesystem::path directory = std::filesystem::absolute(file.parent_path());
        return std::filesystem::proximate(directory, moduleManifest.moduleRoot);
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

    void WriteFilterFile(ModuleManifest& moduleManifest)
    {
        std::filesystem::path filePath = moduleManifest.project->faroRoot / "Project";
        Utility::EnsureDirectory(filePath);
        filePath /= moduleManifest.name + ".vcxproj.filters";

        tinyxml2::XMLDocument doc;

        tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
        projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
        projectElement->SetAttribute("ToolsVersion", "15.0");
        doc.InsertEndChild(projectElement);

        tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");

        std::vector<std::filesystem::path>& sourceFiles = moduleManifest.sourceFiles;

        std::map<std::filesystem::path, std::string> directories;
        for (std::filesystem::path& file : sourceFiles)
        {
            std::filesystem::path directory = GetFileRelativeDirectory(moduleManifest, file);
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

    void WriteSolutionFile(ProjectManifest project, std::vector<CustomCommandInfo>& customCommands)
    {
        std::ofstream stream(project.projectDirectory / (project.projectName + ".sln"));

        stream << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        stream << "# Visual Studio 16\n";
        stream << "MinimumVisualStudioVersion = 10.0.40219.1\n";

        for (CustomCommandInfo& customCommand : customCommands)
        {
            stream << "Project(\"{" + project.uuid + "}\") = \"" + customCommand.name + "\", \"" + (customCommand.projectPath).string() + "\", \"{" + customCommand.uuid + "}\"\n";
            stream << "EndProject\n";
        }

        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            stream << "Project(\"{" + project.uuid + "}\") = \"" + moduleManifest->name + "\", \"" + (moduleManifest->project->faroRoot / "Project" / (moduleManifest->name + ".vcxproj")).string() + "\", \"{" + moduleManifest->uuid + "}\"\n";
            if (!moduleManifest->moduleDependencies.empty())
            {
                stream << "\tProjectSection(ProjectDependencies) = postProject\n";
                for (ModuleManifest* dependency : moduleManifest->moduleDependencies)
                {
                    stream << "\t\t{" + dependency->uuid + "} = {" + dependency->uuid + "}\n";
                }
                stream << "\tEndProjectSection\n";
            }

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
        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            WriteSolutionProjectConfig(stream, moduleManifest->uuid, false);
        }
        for (CustomCommandInfo& customCommand : customCommands)
        {
            WriteSolutionProjectConfig(stream, customCommand.uuid, customCommand.buildByDefault);
        }
        stream << "\tEndGlobalSection\n";
        stream << "EndGlobal\n";



        stream.close();
    }

    inline static  void WriteSolutionProjectConfig(std::ofstream& stream, std::string uuid, bool buildByDefault)
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
                    stream << "\t\t{" << uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.ActiveCfg = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                    if (buildByDefault)
                    {
                        stream << "\t\t{" << uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.Build.0 = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                    }
                }
            }
        }
    }
};
