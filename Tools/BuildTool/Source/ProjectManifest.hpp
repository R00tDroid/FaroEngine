using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Serialization;

public class ProjectManifest
{
    public std::string manifestPath = "";
    public std::string projectDirectory = "";

    // Build directory of this project
    public std::string buildRoot = "";

    public std::string projectName = "";
    public List<ModuleManifest> projectModules = new List<ModuleManifest>();

    public bool Parse(std::string path)
    {
        PerformanceTimer timer = new PerformanceTimer();

        Utility::PrintLineD("Parsing project manifest: " + path);

        manifestPath = path;
        projectDirectory = Directory.GetParent(manifestPath).FullName;
        buildRoot = projectDirectory + "\\.Faro";

        Utility::PrintLineD("Compiling new binary manifest");
        std::string data = "";
        try
        {
            PerformanceTimer readTime = new PerformanceTimer();
            data = File.ReadAllText(path);
            readTime.Stop("Read manifest");
        }
        catch
        {
            Utility::PrintLine("Failed to read project manifest: " + path);
            return false;
        }

        List<std::string> expectedModules = new List<std::string>();

        if (data.Length > 0)
        {
            PerformanceTimer parseTime = new PerformanceTimer();


            JObject rootObject = JObject.Parse(data);

            foreach (JProperty element in rootObject.Properties())
            {
                try
                {
                    std::string propertyName = element.Name.ToLower();

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
                        Utility::PrintLine("Unknown property in manifest: " + element.Name);
                    }
                }
                catch (Exception e)
                {
                    Utility::PrintLine("Failed to parse property in manifest: \"" + element.Name + "\" > " + e.Message + ")");
                }
            }

            parseTime.Stop("Parse project manifest");
        }

        if (expectedModules.Count == 0)
        {
            Utility::PrintLine("Failed to load project manifest: " + path);
            return false;
        }

        projectModules.Clear();
        foreach (std::string modulePath in expectedModules)
        {
            std::string fullModulePath = projectDirectory + "\\" + modulePath;
            
            std::string[] moduleScripts = nullptr;
            try
            {
                moduleScripts = Directory.GetFiles(fullModulePath, "*" + ModuleManifest.moduleFileSuffix);
            }
            catch
            {
                Utility::PrintLine("Failed to find module script in: " + fullModulePath);
                continue;
            }

            if (moduleScripts.Length == 0)
            {
                Utility::PrintLine("No module script found in: " + fullModulePath);
                continue;
            }

            if (moduleScripts.Length > 1)
            {
                Utility::PrintLine("More than one module scripts found in: " + fullModulePath);
                continue;
            }

            ModuleManifest module = new ModuleManifest(moduleScripts[0], this);
            projectModules.Add(module);
        }

        Utility::PrintLineD("Loading modules");
        foreach (ModuleManifest mod in projectModules)
        {
            if (!mod.LoadModule())
            {
                return false;
            }
        }

        Utility::PrintLineD("Resolving module dependencies");
        foreach (ModuleManifest mod in projectModules)
        {
            if (!mod.ResolveDependencies())
            {
                return false;
            }
        }

        Utility::PrintLineD("Parsed project: " + projectName);
        timer.Stop("Parse project");

        return true;
    }
}