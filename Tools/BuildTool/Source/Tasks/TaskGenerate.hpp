using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

public class TaskGenerate : ITask
{
    public override int GetPriority()
    {
        return 3;
    }

    public override bool Run(ProjectManifest project)
    {
        /*Utility::PrintLine("Performing solution generation...");

        PerformanceTimer timer = new PerformanceTimer();
        foreach (ModuleManifest moduleManifest in project.projectModules)
        {
            PerformanceTimer moduleTimer = new PerformanceTimer();

            PerformanceTimer writeTimer = new PerformanceTimer();
            WriteProjectFile(moduleManifest);
            writeTimer.Stop("Project file");

            writeTimer = new PerformanceTimer();
            WriteFilterFile(moduleManifest);
            writeTimer.Stop("Filter file");

            moduleTimer.Stop("Module: " + moduleManifest.name);
        }
        timer.Stop("Generate module projects");

        timer = new PerformanceTimer();
        WriteSolutionFile(project);
        timer.Stop("generate solution file");*/

        return true;
    }

    /*

    private String VSPlatformVersion = "v142";
    private String VSVersion = "16.0";

    private List<string> sourceExtensions = new List<string>() { ".cpp", ".c", "hlsl" };

    public void WriteProjectFile(ModuleManifest moduleManifest)
    {
        string filePath = moduleManifest.project.faroRootDirectory + "\\project\\" + moduleManifest.name + ".vcxproj";

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
                        foreach (string buildTypeName in Enum.GetNames(typeof(BuildType)))
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
                    foreach (string buildTypeName in Enum.GetNames(typeof(BuildType)))
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
                    foreach (string buildTypeName in Enum.GetNames(typeof(BuildType)))
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
                List<string> sourceFiles = moduleManifest.sourceFiles;
                foreach (string file in sourceFiles)
                {
                    string extension = Path.GetExtension(file).ToLower();
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

        writer.Close();
    }

    private static string GetFileRelativeDirectory(ModuleManifest moduleManifest, string file)
    {
        string directory = Directory.GetParent(file).FullName;
        return directory.Replace(moduleManifest.moduleRoot + "\\", "");
    }

    private static List<string> GetAllDirectories(string root)
    {
        List<string> result = new List<string>() { root };
        while (true)
        {
            string current = result[result.Count - 1];
            int pos = current.LastIndexOf('\\');
            if (pos >= 0)
            {
                string parent = current.Substring(0, pos);
                result.Add(parent);
            }
            else
            {
                break;
            }
        }

        return result;
    }

    public void WriteFilterFile(ModuleManifest moduleManifest)
    {
        string filePath = moduleManifest.project.faroRootDirectory + "\\project\\" + moduleManifest.name + ".vcxproj.filters";

        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.IndentChars = "    ";
        XmlWriter writer = XmlWriter.Create(filePath, settings);

        writer.WriteStartDocument();
        writer.WriteStartElement("Project", "http://schemas.microsoft.com/developer/msbuild/2003");
        writer.WriteAttributeString("ToolsVersion", "15.0");

        writer.WriteStartElement("ItemGroup");

        List<string> sourceFiles = moduleManifest.sourceFiles;
        Dictionary<string, string> directories = new Dictionary<string, string>();
        foreach (string file in sourceFiles)
        {
            string directory = GetFileRelativeDirectory(moduleManifest, file);
            List<string> allDirectories = GetAllDirectories(directory);

            foreach (string dir in allDirectories)
            {
                if (!directories.ContainsKey(dir))
                {
                    directories.Add(dir, moduleManifest.project.GUIDs.GetGUID("filter_" + moduleManifest.name + "_" + dir));
                }
            }

            string extension = Path.GetExtension(file).ToLower();
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

        foreach (string directory in directories.Keys)
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
    }

    public void WriteSolutionFile(ProjectManifest project)
    {
        StreamWriter stream = new StreamWriter(File.Open(project.projectDirectory + "\\" + project.projectName + ".sln", FileMode.Create));

        stream.WriteLine("Microsoft Visual Studio Solution File, Format Version 12.00");
        stream.WriteLine("# Visual Studio 16");
        stream.WriteLine("MinimumVisualStudioVersion = 10.0.40219.1");

        string projectGUID = project.GUIDs.GetGUID("project_" + project.projectName);

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

        stream.Close();
    }

    private string GetGUIDForModule(ModuleManifest moduleManifest)
    {
        return moduleManifest.project.GUIDs.GetGUID("module_" + moduleManifest.name);
    }*/
}