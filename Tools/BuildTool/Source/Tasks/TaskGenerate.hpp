#pragma once
#include <filesystem>

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
        /*std::string filePath = moduleManifest.project.faroRootDirectory + "\\project\\" + moduleManifest.name + ".vcxproj";

        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.IndentChars = "    ";
        XmlWriter writer = XmlWriter.Create(filePath, settings);

        List<IToolchain> toolchains = IToolchain.GetToolchains();

        writer.WriteStartDocument();

        {
            writer.WriteStartElement("Project", "http://schemas.microsoft.com/developer/msbuild/2003");
            writer.WriteAttributeString("DefaultTargets", "Build");
            writer.WriteAttributeString("ToolsVersion", "15.0");

            {
                writer.WriteStartElement("ItemGroup");
                writer.WriteAttributeString("Label", "ProjectConfigurations");

                foreach (IToolchain toolchain in toolchains)
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
            }

            {
                writer.WriteStartElement("Import");
                writer.WriteAttributeString("Project", "$(VCTargetsPath)\\Microsoft.Cpp.targets");
                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("ImportGroup");
                writer.WriteAttributeString("Label", "ExtensionTargets");
                writer.WriteEndElement();
            }

            {
                writer.WriteStartElement("ItemGroup");
                List<std::string> sourceFiles = moduleManifest.sourceFiles;
                foreach (std::string file in sourceFiles)
                {
                    std::string extension = Path.GetExtension(file).ToLower();
                    bool shouldCompile = sourceExtensions.Contains(extension);
                    writer.WriteStartElement(shouldCompile ? "ClCompile" : "ClInclude");
                    writer.WriteAttributeString("Include", file);
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }

        writer.WriteEndDocument();

        writer.Close();*/
    }

    static std::filesystem::path GetFileRelativeDirectory(ModuleManifest& moduleManifest, std::filesystem::path file)
    {
        std::filesystem::path directory = std::filesystem::absolute(file.parent_path());
        return std::filesystem::proximate(directory, moduleManifest.moduleRoot);
    }

    static std::vector<std::filesystem::path> GetAllDirectories(std::filesystem::path& root)
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
        /*
        std::string filePath = moduleManifest.project.faroRootDirectory + "\\project\\" + moduleManifest.name + ".vcxproj.filters";

        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.IndentChars = "    ";
        XmlWriter writer = XmlWriter.Create(filePath, settings);

        writer.WriteStartDocument();
        writer.WriteStartElement("Project", "http://schemas.microsoft.com/developer/msbuild/2003");
        writer.WriteAttributeString("ToolsVersion", "15.0");

        writer.WriteStartElement("ItemGroup");

        List<std::string> sourceFiles = moduleManifest.sourceFiles;
        Dictionary<std::string, std::string> directories = new Dictionary<std::string, std::string>();
        foreach (std::string file in sourceFiles)
        {
            std::string directory = GetFileRelativeDirectory(moduleManifest, file);
            List<std::string> allDirectories = GetAllDirectories(directory);

            foreach (std::string dir in allDirectories)
            {
                if (!directories.ContainsKey(dir))
                {
                    directories.Add(dir, moduleManifest.project.GUIDs.GetGUID("filter_" + moduleManifest.name + "_" + dir));
                }
            }

            std::string extension = Path.GetExtension(file).ToLower();
            bool shouldCompile = sourceExtensions.Contains(extension);
            writer.WriteStartElement(shouldCompile ? "ClCompile" : "ClInclude");
            writer.WriteAttributeString("Include", file);
            {
                writer.WriteStartElement("Filter");
                writer.WriteString(directory);
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
        }

        foreach (std::string directory in directories.Keys)
        {
            writer.WriteStartElement("Filter");
            writer.WriteAttributeString("Include", directory);
            {
                writer.WriteStartElement("UniqueIdentifier");
                writer.WriteString("{" + directories[directory] + "}");
                writer.WriteEndElement();
            }
            writer.WriteEndElement();
        }

        writer.WriteEndElement();
        writer.WriteEndDocument();
        writer.Close();
        */
    }

    void WriteSolutionFile(ProjectManifest project)
    {
        /*StreamWriter stream = new StreamWriter(File.Open(project.projectDirectory + "\\" + project.projectName + ".sln", FileMode.Create));

        stream.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
        stream.WriteLine("# Visual Studio 16");
        stream.WriteLine("MinimumVisualStudioVersion = 10.0.40219.1");

        std::string projectGUID = project.GUIDs.GetGUID("project_" + project.projectName);

        foreach (ModuleManifest moduleManifest in project.projectModules)
        {
            stream.WriteLine("Project(\"{" + projectGUID + "}\") = \"" + moduleManifest.name + "\", \".Faro\\project\\" + moduleManifest.name + ".vcxproj\", \"{" + GetGUIDForModule(moduleManifest) + "}\"");
            if (moduleManifest.moduleDependencies.Count > 0)
            {
                stream.WriteLine("\tProjectSection(ProjectDependencies) = postProject");
                foreach (ModuleManifest dependency in moduleManifest.moduleDependencies)
                {
                    stream.WriteLine("\t\t{" + GetGUIDForModule(dependency) + "} = {" + GetGUIDForModule(dependency) + "}");
                }
                stream.WriteLine("\tEndProjectSection");
            }

            stream.WriteLine("EndProject");
        }

        stream.Close();*/
    }

    std::string GetGUIDForModule(ModuleManifest moduleManifest)
    {
        return "";
        //return moduleManifest.project.GUIDs.GetGUID("module_" + moduleManifest.name);
    }
};
