using System;
using System.Collections.Generic;
using System.Diagnostics.Eventing.Reader;
using System.IO;
using System.Text.Json;

public class ProjectManifestData
{
    public String name = "Unnamed project";
    public String[] modules = null;
}

public class ProjectManifest
{
    public string manifestPath = "";
    public string projectDirectory = "";
    public string faroRootDirectory = "";
    public string projectName = "";
    public List<ModuleManifest> projectModules = new List<ModuleManifest>();

    public GUIDManager GUIDs = null;

    public bool Parse(String path)
    {
        PerformanceTimer timer = new PerformanceTimer();

        Utility.PrintLineD("Parsing project manifest: " + path);

        manifestPath = path;
        projectDirectory = Directory.GetParent(manifestPath).FullName;
        faroRootDirectory = projectDirectory + "\\.Faro";

        GUIDs = new GUIDManager(this);

        if (!Directory.Exists(faroRootDirectory))
        {
            DirectoryInfo di = Directory.CreateDirectory(faroRootDirectory);
            di.Attributes = FileAttributes.Directory | FileAttributes.Hidden;
        }

        if (!Directory.Exists(faroRootDirectory + "\\bin"))
        {
            DirectoryInfo di = Directory.CreateDirectory(faroRootDirectory + "\\bin");
        }

        if (!Directory.Exists(faroRootDirectory + "\\project"))
        {
            DirectoryInfo di = Directory.CreateDirectory(faroRootDirectory + "\\project");
        }

        ProjectManifestData manifestData = null;
        String projectBinary = faroRootDirectory + "\\bin\\project.bin";
        if (File.Exists(projectBinary)) //TODO check for change
        {
            PerformanceTimer loadTime = new PerformanceTimer();
            Utility.PrintLineD("Loading binary manifest");
            // Read previous project manifest binary
            BinaryReader reader = new BinaryReader(File.OpenRead(projectBinary));
            manifestData = new ProjectManifestData();
            manifestData.name = reader.ReadString();
            int moduleCount = reader.ReadInt32();
            List<String> modules = new List<String>();
            for(int i = 0; i < moduleCount; i++) 
            {
                modules.Add(reader.ReadString());
            }

            manifestData.modules = modules.ToArray();
            loadTime.Stop("Load manifest binary");
        }
        else
        {
            Utility.PrintLineD("Compiling new binary manifest");
            String data = "";
            try
            {
                PerformanceTimer readTime = new PerformanceTimer();
                data = File.ReadAllText(path);
                readTime.Stop("Read manifest");
            }
            catch
            {
                Utility.PrintLine("Failed to read project manifest: " + path);
                return false;
            }


            if (data.Length > 0)
            {
                PerformanceTimer parseTime = new PerformanceTimer();
                manifestData = new ProjectManifestData();

                JsonDocument rawObject = JsonDocument.Parse(data);
                JsonElement rootObject = rawObject.RootElement;

                JsonElement nameProperty;
                if (rootObject.TryGetProperty("name", out nameProperty))
                {
                    manifestData.name = nameProperty.GetString();
                }

                if (manifestData.modules == null) manifestData.modules = Array.Empty<string>();

                parseTime.Stop("Parse project manifest");
            }

            if (manifestData != null)
            {
                PerformanceTimer saveTime = new PerformanceTimer();
                // Save manifest to binary
                BinaryWriter writer = new BinaryWriter(File.Open(projectBinary, FileMode.Create));
                writer.Write(manifestData.name);
                writer.Write((Int32)manifestData.modules.Length);
                foreach (string modulePath in manifestData.modules)
                {
                    writer.Write(modulePath);
                }
                writer.Close();
                saveTime.Stop("Save manifest binary");
            }
        }

        if (manifestData == null)
        {
            Utility.PrintLine("Failed to load project manifest: " + path);
            return false;
        }

        projectName = manifestData.name;

        projectModules.Clear();
        foreach (String modulePath in manifestData.modules)
        {
            String fullModulePath = projectDirectory + "\\" + modulePath;
            
            String[] moduleScripts = null;
            try
            {
                moduleScripts = Directory.GetFiles(fullModulePath, "*" + ModuleManifest.moduleFileSuffix);
            }
            catch
            {
                Utility.PrintLine("Failed to find module script in: " + fullModulePath);
                continue;
            }

            if (moduleScripts.Length == 0)
            {
                Utility.PrintLine("No module script found in: " + fullModulePath);
                continue;
            }

            if (moduleScripts.Length > 1)
            {
                Utility.PrintLine("More than one module scripts found in: " + fullModulePath);
                continue;
            }

            ModuleManifest module = new ModuleManifest(moduleScripts[0], this);
            projectModules.Add(module);
        }

        if (projectModules.Count != manifestData.modules.Length)
        {
            return false;
        }

        Utility.PrintLineD("Loading modules");
        foreach (ModuleManifest mod in projectModules)
        {
            if (!mod.LoadModule())
            {
                return false;
            }
        }

        Utility.PrintLineD("Resolving module dependencies");
        foreach (ModuleManifest mod in projectModules)
        {
            if (!mod.ResolveDependencies())
            {
                return false;
            }
        }

        Utility.PrintLineD("Parsed project: " + manifestData.name);
        timer.Stop("Parse project");

        return true;
    }
}