#include "VisualStudioGenerator.hpp"
#include "tinyxml2.h"
#include "Utility.hpp"
#include <fstream>
#include <map>

#include "FaroLocation.hpp"
#include "Toolchain.hpp"

bool VSProjectInfo::hasSourceFiles()
{
    return true; //TODO Implement
}

std::vector<std::filesystem::path> VSProjectInfo::getSourceFiles() const
{
    return {}; //TODO Implement
}

std::vector<std::filesystem::path> VSProjectInfo::getIncludePaths() const
{
    return {}; //TODO Implement
}

std::filesystem::path VSProjectInfo::getOutputExecutable(Toolchain*, BuildSetup&)
{
    return {}; //TODO Implement
}

std::filesystem::path VSProjectInfo::getRootDirectory()
{
    return {}; //TODO Implement
}

ModuleManifest* VSProjectInfo::getModuleManifest()
{
    return {}; //TODO Implement
}

std::string VSCustomCommandInfo::getBuildCommand() const
{
    return {}; //TODO Implement
}

std::string VSCustomCommandInfo::getRebuildCommand() const
{
    return {}; //TODO Implement
}

std::string VSCustomCommandInfo::getCleanCommand() const
{
    return {}; //TODO Implement
}

std::string VSModuleInfo::getBuildCommand() const
{
    return {}; //TODO Implement
}

std::string VSModuleInfo::getRebuildCommand() const
{
    return {}; //TODO Implement
}

std::string VSModuleInfo::getCleanCommand() const
{
    return {}; //TODO Implement
}

bool VSModuleInfo::hasSourceFiles()
{
    return {}; //TODO Implement
}

std::vector<std::filesystem::path> VSModuleInfo::getSourceFiles() const
{
    return {}; //TODO Implement
}

std::vector<std::filesystem::path> VSModuleInfo::getIncludePaths() const
{
    return {}; //TODO Implement
}

std::filesystem::path VSModuleInfo::getRootDirectory()
{
    return {}; //TODO Implement
}

std::filesystem::path VSModuleInfo::getOutputExecutable(Toolchain*, BuildSetup&)
{
    return {}; //TODO Implement
}

ModuleManifest* VSModuleInfo::getModuleManifest()
{
    return {}; //TODO Implement
}

bool VisualStudioGenerator::generate(const ProjectManifest* project)
{
    Utility::PrintLine("Performing module generation...");

    std::string faroBuildTool = FaroLocation::buildTool();

    std::filesystem::path vsProjectInfo(project->faroDirectory());
    vsProjectInfo /= "VSProjectInfo";

    std::filesystem::path projectDirectory(project->faroDirectory());
    projectDirectory /= "Project";
    Utility::EnsureDirectory(projectDirectory.string().c_str());

    std::vector<VSProjectInfo*> projectInfoList;
    VSCustomCommandInfo* commandInfo = new VSCustomCommandInfo();
    commandInfo->name = "Build";
    std::string idFile = (vsProjectInfo / (commandInfo->name + "Id.txt")).string();
    std::string projectPath = (projectDirectory / ("Action_" + commandInfo->name + ".vcxproj")).string();
    commandInfo->buildByDefault = true;
    commandInfo->buildCommand = faroBuildTool + " -build -project " + project->manifestPath();
    commandInfo->cleanCommand = faroBuildTool + " -clean -project " + project->manifestPath();
    commandInfo->rebuildCommand = faroBuildTool + " -clean -build -project " + project->manifestPath();
    commandInfo->uuid.resize(UUID_LENGTH);
    Utility::GetCachedUUID(idFile.c_str(), commandInfo->uuid.data());
    commandInfo->projectPath = projectPath.c_str();
    commandInfo->solutionPath = "Project/Actions";
    projectInfoList.push_back(commandInfo);

    commandInfo = new VSCustomCommandInfo();
    commandInfo->name = "Generate";
    idFile = (vsProjectInfo / (commandInfo->name + "Id.txt")).string();
    projectPath = (projectDirectory / ("Action_" + commandInfo->name + ".vcxproj")).string();
    commandInfo->buildByDefault = false;
    commandInfo->buildCommand = faroBuildTool + " -generate -project " + project->manifestPath();
    commandInfo->rebuildCommand = faroBuildTool + " -generate -build -project " + project->manifestPath();
    commandInfo->uuid.resize(UUID_LENGTH);
    Utility::GetCachedUUID(idFile.c_str(), commandInfo->uuid.data());
    commandInfo->projectPath = projectPath.c_str();
    commandInfo->solutionPath = "Project/Actions";
    projectInfoList.push_back(commandInfo);

    Utility::PrintLine("Performing module generation...");

    for (unsigned int moduleIndex = 0; moduleIndex < project->modules(); moduleIndex++)
    {
        ModuleManifest* moduleManifest = project->module(moduleIndex);
        Utility::PrintLine("Discovered module: " + std::string(moduleManifest->name()));

        VSModuleInfo* moduleInfo = new VSModuleInfo();
        moduleInfo->name = moduleManifest->name();
        moduleInfo->module = moduleManifest;
        moduleInfo->uuid = moduleManifest->uuid();
        moduleInfo->projectPath = projectDirectory / ("Module_" + std::string(moduleManifest->name()) + ".vcxproj");
        moduleInfo->solutionPath = "Project/Modules";
        moduleInfo->buildByDefault = false;
        moduleInfo->debuggable = moduleManifest->moduleType() == MT_Executable;
        if (moduleManifest->solutionLocation() != nullptr)
        {
            moduleInfo->solutionPath /= moduleManifest->solutionLocation();
            //TODO Verify if this return a nullptr when empty
        }

        for (unsigned int dependencyIndex = 0; dependencyIndex < moduleManifest->dependencies(); dependencyIndex++)
        {
            ModuleManifest* dependency = moduleManifest->dependency(dependencyIndex);
            moduleInfo->dependencyUuids.push_back(dependency->uuid());
        }

        projectInfoList.push_back(moduleInfo);
    }

    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        writeProjectFile(*VSProjectInfo);
        writeProjectUserFile(*VSProjectInfo);

        if (VSProjectInfo->hasSourceFiles())
        {
            writeFilterFile(*VSProjectInfo);
        }
    }

    Utility::PrintLine("Performing solution generation...");
    writeSolutionFile(project, projectInfoList);

    return false; //TODO Implement VS generation
}

void writeProjectConfigs(tinyxml2::XMLElement* projectElement, const std::string& uuid, const std::string& VSPlatformVersion)
{
    tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");
    projectElement->SetAttribute("Label", "ProjectConfigurations");

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);

        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);

            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                std::string id = target->identifier();
                id += " ";
                id += configurationToString(static_cast<Configuration>(configurationIndex));

                tinyxml2::XMLElement* projectConfig = itemGroup->InsertNewChildElement("ProjectConfiguration");
                projectConfig->SetAttribute("Include", (id + "|Win32").c_str());

                {
                    tinyxml2::XMLElement* configElement = projectConfig->InsertNewChildElement("Configuration");
                    configElement->SetText(id.c_str());
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
    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);

        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);

            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                const char* buildTypeName = configurationToString(static_cast<Configuration>(configurationIndex));

                tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
                propertyGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + std::string(target->identifier()) + " " + buildTypeName + "|Win32'").c_str());
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

/*void writeConfigSection(tinyxml2::XMLElement* projectElement, VSProjectInfo& VSProjectInfo, BuildSetup setup, Toolchain* toolchain)
{
    std::string setupId = setup.identifier();

    tinyxml2::XMLElement* importGroup = projectElement->InsertNewChildElement("ImportGroup");
    importGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + setupId + "|Win32'").c_str());
    importGroup->SetAttribute("Label", "PropertySheets");

    {
        tinyxml2::XMLElement* element = importGroup->InsertNewChildElement("Import");
        element->SetAttribute("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
        element->SetAttribute("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
        element->SetAttribute("Label", "LocalAppDataPlatform");
    }

    tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
    propertyGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + setupId + "|Win32'").c_str());
    {
        tinyxml2::XMLElement* element = propertyGroup->InsertNewChildElement("IncludePath");

        std::string includePaths;

        ModuleManifest* moduleManifest = VSProjectInfo.getModuleManifest();
        if (moduleManifest != nullptr)
        {
            toolchain->prepareModuleForBuild(*VSProjectInfo.getModuleManifest(), platform, buildType);
            std::vector<std::filesystem::path> toolchainIncludes = toolchain->getToolchainIncludes(platform, buildType);

            for (std::filesystem::path& path : toolchainIncludes)
            {
                includePaths += path + ";";
            }
        }

        for (std::filesystem::path& path : VSProjectInfo.getIncludePaths())
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
        //element->SetText((VSProjectInfo.getBuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
        //element->SetText((VSProjectInfo.getRebuildCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
        //element->SetText((VSProjectInfo.getCleanCommand() + " -platform " + platform->platformName + " -" + buildTypeName).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeOutput");

        std::filesystem::path outputPath = VSProjectInfo.getOutputExecutable(toolchain, setup);
        if (!outputPath.empty())
        {
            element->SetText(outputPath.c_str());
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
}*/

void VisualStudioGenerator::writeProjectFile(const VSProjectInfo& project)
{
    tinyxml2::XMLDocument doc(false);

    tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
    projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
    projectElement->SetAttribute("DefaultTargets", "Build");
    projectElement->SetAttribute("ToolsVersion", "Current");
    doc.InsertEndChild(projectElement);

    //TODO Write sections
    //writeProjectConfigs(projectElement, project.uuid, VSPlatformVersion);
    //writeConfigList(projectElement, VSPlatformVersion);

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

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);
            BuildSetup buildSetup;
            buildSetup.target = target;

            for (int ConfigurationIndex = 0; ConfigurationIndex < Configuration::C_ENUMSIZE; ConfigurationIndex++)
            {
                buildSetup.configuration = static_cast<Configuration>(ConfigurationIndex);
                Utility::PrintLineD(buildSetup.identifier());
                //TODO Write section
                //writeConfigSection(projectElement, project, platform, static_cast<BuildType>(buildTypeIndex), toolchain);
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
        std::vector<std::filesystem::path> sourceFiles = project.getSourceFiles();
        for (std::filesystem::path& file : sourceFiles)
        {
            std::string extension = file.extension().string();
            extension = Utility::ToLower(extension);
            bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();
            tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
            fileElement->SetAttribute("Include", file.c_str());
        }
    }

    if (doc.SaveFile(project.projectPath.string().c_str()) != tinyxml2::XML_SUCCESS)
    {
        Utility::PrintLine("Failed to save " + project.projectPath.string());
    }
}

void VisualStudioGenerator::writeProjectUserFile(const VSProjectInfo&)
{
    /*if (!VSProjectInfo.debuggable) return;

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
                    element->SetText(VSProjectInfo.GetOutputExecutable(toolchain, platform, (BuildType)buildTypeIndex).c_str());
                    element = propGroup->InsertNewChildElement("DebuggerFlavor");
                    element->SetText("WindowsLocalDebugger");
                }
            }
        }
    }


    doc.SaveFile(file.c_str());*/
}

std::filesystem::path VisualStudioGenerator::getFileRelativeDirectory(std::filesystem::path, std::filesystem::path)
{
    return {}; //TODO Implement
}

std::vector<std::filesystem::path> VisualStudioGenerator::getDirectoryTree(std::filesystem::path&)
{
    return {}; //TODO Implement
}

void VisualStudioGenerator::writeFilterFile(const VSProjectInfo&)
{
    /*std::filesystem::path filePath = VSProjectInfo.projectPath;
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
        if (!directory.empty() && directory != ".")
        {
            std::vector<std::filesystem::path> allDirectories = GetDirectoryTree(directory);

            for (std::filesystem::path& dir : allDirectories)
            {
                if (directories.find(dir) == directories.end())
                {
                    directories.insert(std::pair<std::filesystem::path, std::string>(dir, Utility::GenerateUUID()));
                }
            }

            std::string extension = file.extension();
            extension = Utility::ToLower(extension);
            bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();

            tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
            fileElement->SetAttribute("Include", file.c_str());

            tinyxml2::XMLElement* filterElement = fileElement->InsertNewChildElement("Filter");
            filterElement->SetText(directory.c_str());
        }
    }

    for (auto& directory : directories)
    {
        tinyxml2::XMLElement* filterElement = itemGroup->InsertNewChildElement("Filter");
        filterElement->SetAttribute("Include", directory.first.c_str());

        tinyxml2::XMLElement* idElement = filterElement->InsertNewChildElement("UniqueIdentifier");
        idElement->SetText(("{" + directory.second + "}").c_str());
    }

    doc.SaveFile(filePath.c_str());*/
}

void VisualStudioGenerator::writeSolutionFile(const ProjectManifest*, std::vector<VSProjectInfo*>)
{
    /*std::ofstream stream(project->manifestDirectory / (project->projectName + ".sln"));

    stream << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
    stream << "# Visual Studio 16\n";
    stream << "MinimumVisualStudioVersion = 10.0.40219.1\n";

    for (VSProjectInfo* VSProjectInfo : projectInfoList)
    {
        stream << "Project(\"{" + project->uuid + "}\") = \"" + VSProjectInfo->name + "\", \"" + (VSProjectInfo->projectPath) + "\", \"{" + VSProjectInfo->uuid + "}\"\n";
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
                solutionDirectories.insert(std::pair<std::filesystem::path, std::string>(solutionDir, Utility::GetCachedUUID(project->manifestDirectory / "VSProjectInfo" / ("Dir" + solutionDir.filename() + "Id.txt"))));
            }
        }
    }

    for (auto& it : solutionDirectories)
    {
        stream << "Project(\"{2150E333-8FDC-42A3-9474-1A3956D46DE8}\") = \"" + it.first.filename() + "\", \"" + it.first.filename() + "\", \"{" + it.second + "}\"\n";
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

    stream.close();*/
}

void VisualStudioGenerator::writeSolutionProjectConfig(std::ofstream&, const VSProjectInfo&)
{
    /*std::vector<Toolchain*> toolchains = Toolchain::getToolchains();
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
    }*/
}
