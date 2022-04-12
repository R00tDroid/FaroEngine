using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Serialization;

public class ProjectManifest
{
    public string manifestPath = "";
    public string projectDirectory = "";

    // Build directory of this project
    public String buildRoot = "";

    public string projectName = "";
    public List<ModuleManifest> projectModules = new List<ModuleManifest>();

    public bool Parse(String path)
    {
        PerformanceTimer timer = new PerformanceTimer();

        Utility.PrintLineD("Parsing project manifest: " + path);

        manifestPath = path;
        projectDirectory = Directory.GetParent(manifestPath).FullName;
        buildRoot = projectDirectory + "\\.Faro";

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

        List<String> expectedModules = new List<String>();

        if (data.Length > 0)
        {
            PerformanceTimer parseTime = new PerformanceTimer();


            JObject rootObject = JObject.Parse(data);

            foreach (JProperty element in rootObject.Properties())
            {
                try
                {
                    string propertyName = element.Name.ToLower();

                    if (propertyName.Equals("name"))
                    {
                        projectName = element.Value.ToString();
                    }
                    else if (propertyName.Equals("modules"))
                    {
                        foreach (JValue moduleEntry in element.Value.Values())
                        {
                            expectedModules.Add(moduleEntry.ToString());
                        }
                    }
                    else if (propertyName.Equals("engineversion"))
                    {
                        //TODO use engine version to check compatibility
                    }
                    else
                    {
                        Utility.PrintLine("Unknown property in manifest: " + element.Name);
                    }
                }
                catch (Exception e)
                {
                    Utility.PrintLine("Failed to parse property in manifest: \"" + element.Name + "\" > " + e.Message + ")");
                }
            }

            parseTime.Stop("Parse project manifest");
        }

        if (expectedModules.Count == 0)
        {
            Utility.PrintLine("Failed to load project manifest: " + path);
            return false;
        }

        projectModules.Clear();
        foreach (String modulePath in expectedModules)
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

        Utility.PrintLineD("Parsed project: " + projectName);
        timer.Stop("Parse project");

        return true;
    }
}