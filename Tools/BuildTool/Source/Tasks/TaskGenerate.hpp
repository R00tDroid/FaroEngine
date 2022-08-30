#pragma once
#include <filesystem>
#include <tinyxml2.h>
#include "ITask.hpp"

class TaskGenerate : public ITask
{
public:
    int GetPriority() override
    {
        return 3;
    }

    bool Run(ProjectManifest& project) override
    {
        for (ModuleManifest* moduleManifest : project.projectModules)
        {
            moduleManifest->Parse();
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

        timer = {};
        WriteSolutionFile(project);
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

        //std::vector<IToolchain> toolchains = IToolchain.GetToolchains();

        tinyxml2::XMLDocument doc;
        {
            tinyxml2::XMLElement* projectElement = doc.NewElement("Project");
            projectElement->SetAttribute("xmlns", "http://schemas.microsoft.com/developer/msbuild/2003");
            projectElement->SetAttribute("DefaultTargets", "Build");
            projectElement->SetAttribute("ToolsVersion", "15.0");
            doc.InsertEndChild(projectElement);

            /* {
                tinyxml2::XMLElement* itemGroup = projectElement->InsertNewChildElement("ItemGroup");
                projectElement->SetAttribute("Label", "ProjectConfigurations");

                for (IToolchain toolchain in toolchains)
                {
                    List<BuildPlatform> platforms = toolchain.GetPlatforms();
                    foreach (BuildPlatform platform in platforms)
                    {
                        foreach (std::string buildTypeName in Enum.GetNames(typeof(BuildType)))
                        {
                            writer.WriteStartElement("ProjectConfiguration");
                            writer.WriteAttributeString("Include", platform.platformName + " " + buildTypeName + "|Win32");

                            {
                                writer.WriteStartElement("Configuration");
                                writer.WriteString(platform.platformName + " " + buildTypeName);
                                writer.WriteEndElement();
                            }
                            {
                                writer.WriteStartElement("Platform");
                                writer.WriteString("Win32");
                                writer.WriteEndElement();
                            }

                            writer.WriteEndElement();
                        }
                    }
                }

                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("PropertyGroup");
                writer.WriteAttributeString("Label", "Globals");

                writer.WriteStartElement("ProjectGuid");
                writer.WriteValue(GetGUIDForModule(moduleManifest));
                writer.WriteEndElement();

                writer.WriteStartElement("PlatformToolset");
                writer.WriteValue(VSPlatformVersion);
                writer.WriteEndElement();

                writer.WriteStartElement("MinimumVisualStudioVersion");
                writer.WriteValue(VSVersion);
                writer.WriteEndElement();

                writer.WriteStartElement("TargetRuntime");
                writer.WriteValue("Native");
                writer.WriteEndElement();

                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("Import");
                writer.WriteAttributeString("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
                writer.WriteEndElement();
            }

            foreach (IToolchain toolchain in toolchains)
            {
                List<BuildPlatform> platforms = toolchain.GetPlatforms();
                foreach (BuildPlatform platform in platforms)
                {
                    foreach (std::string buildTypeName in Enum.GetNames(typeof(BuildType)))
                    {
                        writer.WriteStartElement("PropertyGroup");
                        writer.WriteAttributeString("Condition", "'$(Configuration)|$(Platform)' == '" + platform.platformName + " " + buildTypeName + "|Win32'");
                        writer.WriteAttributeString("Label", "Configuration");

                        {
                            writer.WriteStartElement("ConfigurationType");
                            writer.WriteString("Makefile");
                            writer.WriteEndElement();
                        }
                        {
                            writer.WriteStartElement("PlatformToolset");
                            writer.WriteString(VSPlatformVersion);
                            writer.WriteEndElement();
                        }

                        writer.WriteEndElement();
                    }
                }
            }

            {
                writer.WriteStartElement("Import");
                writer.WriteAttributeString("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("ImportGroup");
                writer.WriteAttributeString("Label", "ExtensionTargets");
                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("PropertyGroup");
                writer.WriteAttributeString("Label", "UserMacros");
                writer.WriteEndElement();
            }

            foreach (IToolchain toolchain in toolchains)
            {
                List<BuildPlatform> platforms = toolchain.GetPlatforms();
                foreach (BuildPlatform platform in platforms)
                {
                    foreach (std::string buildTypeName in Enum.GetNames(typeof(BuildType)))
                    {
                        writer.WriteStartElement("ImportGroup");
                        writer.WriteAttributeString("Condition", "'$(Configuration)|$(Platform)' == '" + platform.platformName + " " + buildTypeName + "|Win32'");
                        writer.WriteAttributeString("Label", "PropertySheets");
                        {
                            writer.WriteStartElement("Import");
                            writer.WriteAttributeString("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
                            writer.WriteAttributeString("Condition", "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
                            writer.WriteAttributeString("Label", "LocalAppDataPlatform");
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();

                        writer.WriteStartElement("PropertyGroup");
                        writer.WriteAttributeString("Condition", "'$(Configuration)|$(Platform)' == '" + platform.platformName + " " + buildTypeName + "|Win32'");
                        {
                            writer.WriteStartElement("IncludePath");
                            writer.WriteString("$(VC_IncludePath);$(WindowsSDK_IncludePath);");
                            writer.WriteEndElement();

                            writer.WriteStartElement("ReferencePath");
                            writer.WriteEndElement();
                            writer.WriteStartElement("LibraryPath");
                            writer.WriteEndElement();
                            writer.WriteStartElement("LibraryWPath");
                            writer.WriteEndElement();
                            writer.WriteStartElement("SourcePath");
                            writer.WriteEndElement();
                            writer.WriteStartElement("ExcludePath");
                            writer.WriteEndElement();

                            writer.WriteStartElement("OutDir");
                            writer.WriteString("$(ProjectDir)\\..\\Bin");
                            writer.WriteEndElement();

                            writer.WriteStartElement("IntDir");
                            writer.WriteString("$(ProjectDir)\\Intermediate");
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakeBuildCommandLine");
                            writer.WriteString("faro-build -build -project " + moduleManifest.project.manifestPath + " -module " + moduleManifest.name + " -platform \"" + platform.platformName + "\" -config " + buildTypeName);
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakeReBuildCommandLine");
                            writer.WriteString("faro-build -clean -build -project " + moduleManifest.project.manifestPath + " -module " + moduleManifest.name + " -platform \"" + platform.platformName + "\" -config " + buildTypeName);
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakeCleanCommandLine");
                            writer.WriteString("faro-build -clean -project " + moduleManifest.project.manifestPath + " -module " + moduleManifest.name + " -platform \"" + platform.platformName + "\" -config " + buildTypeName);
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakeOutput");
                            writer.WriteString("");
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakeIncludeSearchPath");
                            writer.WriteString(""); //TODO set include paths
                            writer.WriteEndElement();

                            writer.WriteStartElement("NMakePreprocessorDefinitions");
                            writer.WriteString(""); //TODO set preprocessor defines
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();
                    }
                }
            }*/

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
                    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
                    bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();
                    tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
                    fileElement->SetAttribute("Include", file.string().c_str());
                }
            }
        }

        doc.SaveFile(filePath.string().c_str());
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
            std::vector<std::filesystem::path> allDirectories = GetDirectoryTree(directory);

            for (std::filesystem::path& dir : allDirectories)
            {
                if (directories.find(dir) == directories.end())
                {
                    directories.insert(std::pair<std::filesystem::path, std::string>(dir, "sdf"));
                    //directories.Add(dir, moduleManifest.project.GUIDs.GetGUID("filter_" + moduleManifest.name + "_" + dir));
                }
            }

            std::string extension = file.extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), tolower);
            bool shouldCompile = std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();

            tinyxml2::XMLElement* fileElement = itemGroup->InsertNewChildElement(shouldCompile ? "ClCompile" : "ClInclude");
            fileElement->SetAttribute("Include", file.string().c_str());

            tinyxml2::XMLElement* filterElement = fileElement->InsertNewChildElement("Filter");
            filterElement->SetText(directory.string().c_str());
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

    void WriteSolutionFile(ProjectManifest project)
    {
        std::ofstream stream(project.projectDirectory / (project.projectName + ".sln"));

        stream << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
        stream << "# Visual Studio 16\n";
        stream << "MinimumVisualStudioVersion = 10.0.40219.1\n";

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

        stream.close();
    }
};
