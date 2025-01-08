#include "VisualStudioGenerator.hpp"
#include "tinyxml2.h"
#include "Utility.hpp"
#include <fstream>
#include <map>

#include "Toolchain.hpp"

bool VisualStudioGenerator::generate(const ProjectManifest*)
{
    Utility::PrintLine("Performing module generation...");

    std::vector<VSProjectInfo*> projectInfoList;
    VSCustomCommandInfo* commandInfo = new VSCustomCommandInfo();
    commandInfo->name = "Build";
    commandInfo->buildByDefault = true;
    commandInfo->buildCommand = faroBuildTool.string() + " -build -project " + taskInfo.projectManifest->manifestPath.string();
    commandInfo->cleanCommand = faroBuildTool.string() + " -clean -project " + taskInfo.projectManifest->manifestPath.string();
    commandInfo->rebuildCommand = faroBuildTool.string() + " -clean -build -project " + taskInfo.projectManifest->manifestPath.string();
    commandInfo->uuid = Utility::GetCachedUUID(taskInfo.projectManifest->faroDirectory / "VSProjectInfo" / (commandInfo->name + "Id.txt"));
    commandInfo->projectPath = taskInfo.projectManifest->faroDirectory / "Project" / (commandInfo->name + ".vcxproj");
    commandInfo->solutionPath = "Project/Actions";
    projectInfoList.push_back(commandInfo);

    commandInfo = new VSCustomCommandInfo();
    commandInfo->name = "Generate";
    commandInfo->buildByDefault = false;
    commandInfo->buildCommand = faroBuildTool.string() + " -generate -project " + taskInfo.projectManifest->manifestPath.string();
    commandInfo->rebuildCommand = faroBuildTool.string() + " -generate -build -project " + taskInfo.projectManifest->manifestPath.string();
    commandInfo->uuid = Utility::GetCachedUUID(taskInfo.projectManifest->faroDirectory / "VSProjectInfo" / (commandInfo->name + "Id.txt"));
    commandInfo->projectPath = taskInfo.projectManifest->faroDirectory / "Project" / (commandInfo->name + ".vcxproj");
    commandInfo->solutionPath = "Project/Actions";
    projectInfoList.push_back(commandInfo);

    Utility::PrintLine("Performing module generation...");

    for (std::filesystem::path& modulePath : taskInfo.projectManifest->modulesPaths)
    {
        ModuleManifest* moduleManifest = ModuleManifest::GetLoadedModule(modulePath);
        Utility::PrintLine("Discovered module: " + moduleManifest->name);

        PerformanceTimer moduleTimer;

        VSModuleInfo* moduleInfo = new VSModuleInfo();
        moduleInfo->name = moduleManifest->name;
        moduleInfo->module = moduleManifest;
        moduleInfo->uuid = moduleManifest->uuid;
        moduleInfo->projectPath = moduleManifest->project->faroDirectory / "Project" / (moduleManifest->name + ".vcxproj");
        moduleInfo->solutionPath = "Project/Modules";
        moduleInfo->buildByDefault = false;
        moduleInfo->debuggable = moduleManifest->type == MT_Executable;
        if (!moduleManifest->solutionLocation.empty())
        {
            moduleInfo->solutionPath /= moduleManifest->solutionLocation;
        }

        for (std::filesystem::path& dependencyPath : moduleManifest->moduleDependencies)
        {
            ModuleManifest* dependency = ModuleManifest::GetLoadedModule(dependencyPath);
            moduleInfo->dependencyUuids.push_back(dependency->uuid);
        }

        projectInfoList.push_back(moduleInfo);

        moduleTimer.Stop("Module: " + moduleManifest->name);
    }

    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        writeProjectFile(*VSProjectInfo);
        writeProjectUserFile(*VSProjectInfo);

        if (VSProjectInfo->HasSourceFiles())
        {
            writeFilterFile(*VSProjectInfo);
        }
    }

    Utility::PrintLine("Performing solution generation...");
    writeSolutionFile(taskInfo.projectManifest, projectInfoList);

    return false; //TODO Implement VS generation
}

void writeProjectConfigs(tinyxml2::XMLElement* projectElement, const std::string& uuid, const std::string& VSPlatformVersion)
{
    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();

    tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");
    projectElement->SetAttribute("Label", "ProjectConfigurations");

    for (Toolchain* toolchain : toolchains)
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

    {
        tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
        propertyGroup->SetAttribute("Label", "Globals");

        tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("ProjectGuid");
        element->SetText(uuid.c_str());

        element = propertyGroup->InsertNewChildElement("PlatformToolset");
        element->SetText(VSPlatformVersion.c_str());

        element = propertyGroup->InsertNewChildElement("TargetRuntime");
        element->SetText("Native");
    }

    {
        tinyxml2::XMLElement* importElement = projectElement->InsertNewChildElement("Import");
        importElement->SetAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
    }
}

void writeConfigList(tinyxml2::XMLElement* projectElement, const std::string& VSPlatformVersion)
{
    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();
    for (Toolchain* toolchain : toolchains)
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
}

void writeConfigSection(tinyxml2::XMLElement* projectElement, TaskGenerate::VSProjectInfo& VSProjectInfo, BuildPlatform* platform, BuildType buildType, IToolchain* toolchain)
{
    const char* buildTypeName = BuildTypeNames[buildType];

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

        std::string includePaths;

        ModuleManifest* moduleManifest = VSProjectInfo.GetModuleManifest();
        if (moduleManifest != nullptr)
        {
            toolchain->PrepareModuleForBuild(*VSProjectInfo.GetModuleManifest(), platform, buildType);
            std::vector<std::filesystem::path> toolchainIncludes = toolchain->GetToolchainIncludes(platform, buildType);

            for (std::filesystem::path& path : toolchainIncludes)
            {
                includePaths += path.string() + ";";
            }
        }

        for (std::filesystem::path& path : VSProjectInfo.GetIncludePaths())
        {
            includePaths += path.string() + ";";
        }

        element->SetText(includePaths.c_str());

        propertyGroup->InsertNewChildElement("ReferencePath");
        propertyGroup->InsertNewChildElement("LibraryPath");
        propertyGroup->InsertNewChildElement("LibraryWPath");
        propertyGroup->InsertNewChildElement("SourcePath");
        propertyGroup->InsertNewChildElement("ExcludePath");

        std::filesystem::path faroBuildTool = Utility::GetExecutablePath();

        element = propertyGroup->InsertNewChildElement("OutDir");
        element->SetText("$(ProjectDir)\\..\\Bin");

        element = propertyGroup->InsertNewChildElement("IntDir");
        element->SetText("$(ProjectDir)\\Intermediate");

        element = propertyGroup->InsertNewChildElement("NMakeBuildCommandLine");
        element->SetText((VSProjectInfo.GetBuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
        element->SetText((VSProjectInfo.GetRebuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
        element->SetText((VSProjectInfo.GetCleanCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeOutput");

        std::filesystem::path outputPath = VSProjectInfo.getOutputExecutable(toolchain, platform, buildType);
        if (!outputPath.empty())
        {
            element->SetText(outputPath.string().c_str());
        }

        element = propertyGroup->InsertNewChildElement("NMakeIncludeSearchPath");
        element->SetText(includePaths.c_str());

        element = propertyGroup->InsertNewChildElement("NMakePreprocessorDefinitions");

        std::string preprocessorDefines;
        for (std::string& path : toolchain->GetPreprocessorDefines(VSProjectInfo.GetModuleManifest(), platform, buildType))
        {
            preprocessorDefines += path + ";";
        }

        element->SetText(preprocessorDefines.c_str());
    }
}

void VisualStudioGenerator::writeProjectFile(VSProjectInfo& VSProjectInfo)
{
    Utility::EnsureDirectory(VSProjectInfo.projectPath.parent_path());

    tinyxml2::XMLDocument doc(false);

    tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
    projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
    projectElement->SetAttribute("DefaultTargets", "Build");
    projectElement->SetAttribute("ToolsVersion", "Current");
    doc.InsertEndChild(projectElement);

    writeProjectConfigs(projectElement, VSProjectInfo.uuid, VSPlatformVersion);

    writeConfigList(projectElement, VSPlatformVersion);

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

    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();
    for (Toolchain* toolchain : toolchains)
    {
        std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
        for (BuildPlatform* platform : platforms)
        {
            for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
            {
                writeConfigSection(projectElement, VSProjectInfo, platform, static_cast<BuildType>(buildTypeIndex), toolchain);
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
        std::vector<std::filesystem::path> sourceFiles = VSProjectInfo.GetSourceFiles();
        for (std::filesystem::path& file : sourceFiles)
        {
            std::string extension = file.extension().string();
            extension = Utility::ToLower(extension);
            bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();
            tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
            fileElement->SetAttribute("Include", file.string().c_str());
        }
    }


    doc.SaveFile(VSProjectInfo.projectPath.string().c_str());
}

void VisualStudioGenerator::writeProjectUserFile(VSProjectInfo& VSProjectInfo)
{
    if (!VSProjectInfo.debuggable) return;

    std::filesystem::path file = VSProjectInfo.projectPath;
    file.replace_extension(".vcxproj.user");

    Utility::EnsureDirectory(file.parent_path());

    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();

    tinyxml2::XMLDocument doc(false);
    {
        tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
        projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
        projectElement->SetAttribute("ToolsVersion", "Current");
        doc.InsertEndChild(projectElement);


        for (Toolchain* toolchain : toolchains)
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
                    element->SetText(VSProjectInfo.GetOutputExecutable(toolchain, platform, (BuildType)buildTypeIndex).string().c_str());
                    element = propGroup->InsertNewChildElement("DebuggerFlavor");
                    element->SetText("WindowsLocalDebugger");
                }
            }
        }
    }


    doc.SaveFile(file.string().c_str());
}

void VisualStudioGenerator::writeFilterFile(VSProjectInfo& VSProjectInfo)
{
    std::filesystem::path filePath = VSProjectInfo.projectPath;
    filePath.replace_extension(".vcxproj.filters");

    tinyxml2::XMLDocument doc(false);

    tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
    projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
    projectElement->SetAttribute("ToolsVersion", "Current");
    doc.InsertEndChild(projectElement);

    tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");

    std::vector<std::filesystem::path> sourceFiles = VSProjectInfo.GetSourceFiles();

    std::map<std::filesystem::path, std::string> directories;
    for (std::filesystem::path& file : sourceFiles)
    {
        std::filesystem::path directory = GetFileRelativeDirectory(VSProjectInfo.GetRootDirectory(), file);
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

void VisualStudioGenerator::writeSolutionFile(ProjectManifest* project, std::vector<VSProjectInfo*> projectInfoList)
{
    std::ofstream stream(project->manifestDirectory / (project->projectName + ".sln"));

    stream << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
    stream << "# Visual Studio 16\n";
    stream << "MinimumVisualStudioVersion = 10.0.40219.1\n";

    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        stream << "Project(\"{" + project->uuid + "}\") = \"" + VSProjectInfo->name + "\", \"" + (VSProjectInfo->projectPath).string() + "\", \"{" + VSProjectInfo->uuid + "}\"\n";
        if (!VSProjectInfo->dependencyUuids.empty())
        {
            stream << "\tProjectSection(ProjectDependencies) = postProject\n";
            for (std::string& dependency : VSProjectInfo->dependencyUuids)
            {
                stream << "\t\t{" << dependency << "} = {" << dependency << "}\n";
            }
            stream << "\tEndProjectSection\n";
        }
        stream << "EndProject\n";
    }

    std::map<std::filesystem::path, std::string> solutionDirectories;
    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        VSProjectInfo->solutionPath.make_preferred();
        VSProjectInfo->solutionPath = std::filesystem::weakly_canonical(VSProjectInfo->solutionPath);

        std::vector<std::filesystem::path> tree = GetDirectoryTree(VSProjectInfo->solutionPath);
        for (std::filesystem::path& solutionDir : tree)
        {
            auto it = solutionDirectories.find(solutionDir);
            if (it == solutionDirectories.end())
            {
                solutionDirectories.insert(std::pair<std::filesystem::path, std::string>(solutionDir, Utility::GetCachedUUID(project->manifestDirectory / "VSProjectInfo" / ("Dir" + solutionDir.filename().string() + "Id.txt"))));
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
    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();
    for (Toolchain* toolchain : toolchains)
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
    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        writeSolutionProjectConfig(stream, *VSProjectInfo);
    }
    stream << "\tEndGlobalSection\n";

    stream << "\tGlobalSection(NestedProjects) = preSolution\n";
    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        stream << "\t\t{" << VSProjectInfo->uuid << "} = {" << solutionDirectories[VSProjectInfo->solutionPath] << "}\n";
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

void VisualStudioGenerator::writeSolutionProjectConfig(std::ofstream& stream, VSProjectInfo& VSProjectInfo)
{
    std::vector<Toolchain*> toolchains = Toolchain::getToolchains();
    for (Toolchain* toolchain : toolchains)
    {
        std::vector<BuildPlatform*> platforms = toolchain->GetPlatforms();
        for (BuildPlatform* platform : platforms)
        {
            for (int buildTypeIndex = 0; buildTypeIndex < BuildType::ENUMSIZE; buildTypeIndex++)
            {
                const char* buildTypeName = BuildTypeNames[buildTypeIndex];
                stream << "\t\t{" << VSProjectInfo.uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.ActiveCfg = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                if (VSProjectInfo.buildByDefault)
                {
                    stream << "\t\t{" << VSProjectInfo.uuid << "}." << platform->platformName << " " << buildTypeName << "|x86.Build.0 = " << platform->platformName << " " << buildTypeName << "|Win32\n";
                }
            }
        }
    }
}
