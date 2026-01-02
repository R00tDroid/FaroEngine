#include "VisualStudioGenerator.hpp"
#include "tinyxml2.h"
#include "Utility.hpp"
#include <fstream>
#include <map>

#include "FaroLocation.hpp"
#include "MSVCInfo/MSVCInfo.hpp"
#include "Toolchain.hpp"

const ProjectManifest* VSProjectInfo::getProjectManifest() const
{
    return project;
}

std::string VSCustomCommandInfo::getBuildCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -build";
}

std::string VSCustomCommandInfo::getRebuildCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -clean -build";
}

std::string VSCustomCommandInfo::getCleanCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -clean";
}

std::string VSModuleInfo::getBuildCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -build -module " + module->name();
}

std::string VSModuleInfo::getRebuildCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -clean -build -module " + module->name();
}

std::string VSModuleInfo::getCleanCommand() const
{
    return "\"" + std::string(FaroLocation::buildTool()) + "\" -clean -module " + module->name();
}

bool VSModuleInfo::hasSourceFiles() const
{
    return module->sourceFiles() > 0;
}

std::vector<std::filesystem::path> VSModuleInfo::getSourceFiles() const
{
    unsigned int sourceCount = module->sourceFiles();
    std::vector<std::filesystem::path> paths;
    paths.reserve(sourceCount);
    for (unsigned int i = 0; i < sourceCount; i++)
    {
        paths.emplace_back(module->sourceFile(i));
    }
    return paths;
}

std::vector<std::filesystem::path> VSModuleInfo::getIncludePaths() const
{
    return module->moduleIncludes();
}

std::filesystem::path VSModuleInfo::getRootDirectory() const
{
    return module->manifestDirectory();
}

std::filesystem::path VSModuleInfo::getOutputExecutable(const Toolchain*, const BuildSetup&) const
{
    return {}; //TODO Implement
}

std::filesystem::path projectFilePath(const std::filesystem::path& directory, const ModuleManifest* moduleManifest)
{
    return directory / ("Module_" + std::string(moduleManifest->name()) + ".vcxproj");
}

bool VisualStudioGenerator::generate(const ProjectManifest* project)
{
    Utility::PrintLine("Performing module generation...");

    unsigned int msvcVersions = msvcInstallations();
    Utility::PrintLineD("Found " + std::to_string(msvcVersions) + " MSVC versions");
    if (msvcVersions == 0)
    {
        Utility::PrintLine("Failed to find MSVC toolchain");
        return false;
    }
    msvcVersion = msvcInstallation(0);
    Utility::PrintLineD("Using MSVC " + msvcVersion.redistVersion + " (" + msvcVersion.root + ")");

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
    commandInfo->buildByDefault = false;
    commandInfo->buildCommand = faroBuildTool + " -build -project " + project->manifestPath();
    commandInfo->cleanCommand = faroBuildTool + " -clean -project " + project->manifestPath();
    commandInfo->rebuildCommand = faroBuildTool + " -clean -build -project " + project->manifestPath();
    commandInfo->uuid.resize(UUID_LENGTH);
    Utility::GetCachedUUID(idFile.c_str(), commandInfo->uuid.data());
    commandInfo->projectPath = projectPath.c_str();
    commandInfo->solutionPath = "Project/Actions";
    commandInfo->project = project;
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
    commandInfo->project = project;
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
        moduleInfo->projectPath = projectFilePath(projectDirectory, moduleManifest);
        moduleInfo->solutionPath = "Project/Modules";
        moduleInfo->buildByDefault = true;
        moduleInfo->debuggable = moduleManifest->moduleType() == MT_Executable;
        moduleInfo->project = project;
        if (moduleManifest->solutionLocation() != nullptr)
        {
            moduleInfo->solutionPath /= moduleManifest->solutionLocation();
        }

        for (unsigned int dependencyIndex = 0; dependencyIndex < moduleManifest->dependencies(); dependencyIndex++)
        {
            ModuleManifest* dependency = moduleManifest->dependency(dependencyIndex);
            moduleInfo->dependencyProjects.push_back(projectFilePath(projectDirectory, dependency));
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

    return true;
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
            BuildSetup buildSetup;
            buildSetup.target = target;

            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                buildSetup.configuration = static_cast<Configuration>(configurationIndex);
                std::string id = buildSetup.identifier();

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
            BuildSetup buildSetup;
            buildSetup.target = target;

            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                buildSetup.configuration = static_cast<Configuration>(configurationIndex);

                std::string setupId = buildSetup.identifier();

                tinyxml2::XMLElement* propertyGroup = projectElement->InsertNewChildElement("PropertyGroup");
                propertyGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)' == '" + setupId + "|Win32'").c_str());
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

void writeConfigSection(tinyxml2::XMLElement* projectElement, const VSProjectInfo& project, const BuildSetup& setup, const Toolchain* toolchain)
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

        std::string includeDirectories;

        for (unsigned int i = 0; i < toolchain->includePaths(); i++)
        {
            includeDirectories += std::string(toolchain->includePath(i)) + ";";
        }
        for (std::filesystem::path& path : project.getIncludePaths())
        {
            includeDirectories += path.string() + ";";
        }

        element->SetText(includeDirectories.c_str());

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

        std::string config = std::string(" -platform ") + setup.target->identifier() + " -" + configurationToString(setup.configuration) + " -path \"" + project.getProjectManifest()->manifestPath() + "\"";

        element = propertyGroup->InsertNewChildElement("NMakeBuildCommandLine");
        element->SetText((project.getBuildCommand() + config).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeReBuildCommandLine");
        element->SetText((project.getRebuildCommand() + config).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeCleanCommandLine");
        element->SetText((project.getCleanCommand() + config).c_str());

        element = propertyGroup->InsertNewChildElement("NMakeOutput");

        std::filesystem::path outputPath = project.getOutputExecutable(toolchain, setup);
        if (!outputPath.empty())
        {
            element->SetText(outputPath.c_str());
        }

        element = propertyGroup->InsertNewChildElement("NMakeIncludeSearchPath");
        element->SetText(includeDirectories.c_str());

        element = propertyGroup->InsertNewChildElement("NMakePreprocessorDefinitions");

        std::string preprocessorDefines;
        for (const std::string& define : Toolchain::getSetupDefines(setup))
        {
            preprocessorDefines += define + ";";
        }
        element->SetText(preprocessorDefines.c_str());
    }
}

void VisualStudioGenerator::writeProjectFile(const VSProjectInfo& project)
{
    tinyxml2::XMLDocument doc(false);

    tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
    projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
    projectElement->SetAttribute("DefaultTargets", "Build");
    projectElement->SetAttribute("ToolsVersion", "Current");
    doc.InsertEndChild(projectElement);

    writeProjectConfigs(projectElement, project.uuid, msvcVersion.redistVersion);
    writeConfigList(projectElement, msvcVersion.redistVersion);

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
                toolchain->prepare(buildSetup);
                buildSetup.configuration = static_cast<Configuration>(ConfigurationIndex);
                Utility::PrintLineD(buildSetup.identifier());
                writeConfigSection(projectElement, project, buildSetup, toolchain);
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
            tinyxml2::XMLElement* fileElement = nullptr;
            if (Utility::IsSourceFile(file.string().c_str()))
            {
                fileElement = itemGroup->InsertNewChildElement("ClCompile");
            }
            else if (Utility::IsIncludeFile(file.string().c_str()))
            {
                fileElement = itemGroup->InsertNewChildElement("ClInclude");
            }
            else
            {
                Utility::PrintLine("Failed to determine source file type: " + file.string());
            }

            if (fileElement != nullptr)
            {
                fileElement->SetAttribute("Include", file.string().c_str());
            }
        }
    }

    if (doc.SaveFile(project.projectPath.string().c_str()) != tinyxml2::XML_SUCCESS)
    {
        Utility::PrintLine("Failed to save " + project.projectPath.string());
    }
}

void VisualStudioGenerator::writeProjectUserFile(const VSProjectInfo& project)
{
    if (!project.debuggable) return;

    std::filesystem::path file = project.projectPath;
    file.replace_extension(".vcxproj.user");

    tinyxml2::XMLDocument doc(false);
    {
        tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
        projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
        projectElement->SetAttribute("ToolsVersion", "Current");
        doc.InsertEndChild(projectElement);


        for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
        {
            Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
            for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
            {
                Target* target = toolchain->target(targetIndex);
                BuildSetup buildSetup;
                buildSetup.target = target;

                for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
                {
                    buildSetup.configuration = static_cast<Configuration>(configurationIndex);
                    tinyxml2::XMLElement* propGroup = projectElement->InsertNewChildElement("PropertyGroup");
                    propGroup->SetAttribute("Condition", ("'$(Configuration)|$(Platform)'=='" + buildSetup.identifier() + "|Win32'").c_str());

                    tinyxml2::XMLElement* element = propGroup->InsertNewChildElement("LocalDebuggerCommand");
                    element->SetText(project.getOutputExecutable(toolchain, buildSetup).c_str());
                    element = propGroup->InsertNewChildElement("DebuggerFlavor");
                    element->SetText("WindowsLocalDebugger");
                }
            }
        }
    }


    doc.SaveFile(file.string().c_str());
}

std::filesystem::path VisualStudioGenerator::getFileRelativeDirectory(std::filesystem::path root, std::filesystem::path file)
{
    std::filesystem::path directory = std::filesystem::absolute(file.parent_path());
    return std::filesystem::proximate(directory, root);
}

std::vector<std::filesystem::path> VisualStudioGenerator::getDirectoryTree(std::filesystem::path& root)
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

void VisualStudioGenerator::writeFilterFile(const VSProjectInfo& project)
{
    std::filesystem::path filePath = project.projectPath;
    filePath.replace_extension(".vcxproj.filters");

    tinyxml2::XMLDocument doc(false);

    tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
    projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
    projectElement->SetAttribute("ToolsVersion", "Current");
    doc.InsertEndChild(projectElement);

    tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");

    std::vector<std::filesystem::path> sourceFiles = project.getSourceFiles();

    std::map<std::filesystem::path, std::string> directories;
    for (std::filesystem::path& file : sourceFiles)
    {
        std::filesystem::path directory = getFileRelativeDirectory(project.getRootDirectory(), file);
        if (!directory.empty() && directory != ".")
        {
            std::vector<std::filesystem::path> allDirectories = getDirectoryTree(directory);

            for (std::filesystem::path& dir : allDirectories)
            {
                if (directories.find(dir) == directories.end())
                {
                    char uuid[UUID_LENGTH];
                    Utility::GenerateUUID(uuid); //TODO Cache uuid
                    directories.insert(std::pair<std::filesystem::path, std::string>(dir, std::string(uuid, UUID_LENGTH)));
                }
            }

            tinyxml2::XMLElement* fileElement = nullptr;
            if (Utility::IsSourceFile(file.string().c_str()))
            {
                fileElement = itemGroup->InsertNewChildElement("ClCompile");
            }
            else if (Utility::IsIncludeFile(file.string().c_str()))
            {
                fileElement = itemGroup->InsertNewChildElement("ClInclude");
            }
            else
            {
                Utility::PrintLine("Failed to determine source file type: " + file.string());
            }

            if (fileElement != nullptr)
            {
                fileElement->SetAttribute("Include", file.string().c_str());

                tinyxml2::XMLElement* filterElement = fileElement->InsertNewChildElement("Filter");
                filterElement->SetText(directory.string().c_str());
            }
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

void VisualStudioGenerator::writeSolutionFile(const ProjectManifest* project, std::vector<VSProjectInfo*> projectInfoList)
{
    tinyxml2::XMLDocument doc(false);
    std::filesystem::path path = std::filesystem::path(project->manifestDirectory()) / (std::string(project->projectName()) + ".slnx");

    tinyxml2::XMLElement* solutionElement = doc.NewElement("Solution");
    doc.InsertEndChild(solutionElement);

    tinyxml2::XMLElement* configurationsElement = solutionElement->InsertNewChildElement("Configurations");

    tinyxml2::XMLElement* platformElement = configurationsElement->InsertNewChildElement("Platform");
    platformElement->SetAttribute("Name", "x86");

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            Target* target = toolchain->target(targetIndex);
            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                Configuration configuration = static_cast<Configuration>(configurationIndex);
                std::string targetName = target->displayName();
                std::string configName = configurationToString(configuration);
                std::string displayName = targetName + " " + configName;

                tinyxml2::XMLElement* buildTypeElement = configurationsElement->InsertNewChildElement("BuildType");
                buildTypeElement->SetAttribute("Name", displayName.c_str());
            }
        }
    }

    for (VSProjectInfo* projectInfo : projectInfoList)
    {
        writeSolutionProjectConfig(solutionElement, projectInfo);
    }

    doc.SaveFile(path.string().c_str());
}

void VisualStudioGenerator::writeSolutionProjectConfig(tinyxml2::XMLElement* solutionElement, const VSProjectInfo* projectInfo)
{
    tinyxml2::XMLElement* projectElement = solutionElement->InsertNewChildElement("Project");
    projectElement->SetAttribute("Path", projectInfo->projectPath.string().c_str());
    projectElement->SetAttribute("Id", projectInfo->uuid.c_str());

    for (unsigned int toolchainIndex = 0; toolchainIndex < Toolchain::toolchains(); toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        for (unsigned int targetIndex = 0; targetIndex < toolchain->targets(); targetIndex++)
        {
            BuildSetup setup;
            setup.target = toolchain->target(targetIndex);

            for (int configurationIndex = 0; configurationIndex < Configuration::C_ENUMSIZE; configurationIndex++)
            {
                setup.configuration = static_cast<Configuration>(configurationIndex);
                std::string configName = configurationToString(setup.configuration);
                std::string setupName = setup.identifier();
                std::string displayName = std::string(setup.target->displayName()) + " " + configName;

                tinyxml2::XMLElement* buildTypeElement = projectElement->InsertNewChildElement("BuildType");
                buildTypeElement->SetAttribute("Solution", (displayName + "|*").c_str());
                buildTypeElement->SetAttribute("Project", setupName.c_str());
            }
        }
    }

    if (!projectInfo->buildByDefault)
    {
        tinyxml2::XMLElement* buildElement = projectElement->InsertNewChildElement("Build");
        buildElement->SetAttribute("Solution", "*|*");
        buildElement->SetAttribute("Project", "false");
    }

    for (const std::filesystem::path& dependency : projectInfo->dependencyProjects)
    {
        tinyxml2::XMLElement* dependencyElement = projectElement->InsertNewChildElement("BuildDependency");
        dependencyElement->SetAttribute("Project", dependency.string().c_str());
    }
}
