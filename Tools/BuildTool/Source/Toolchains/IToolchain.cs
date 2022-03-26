using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;

public enum BuildType
{
    Debug,
    Development,
    Release
}

public class BuildPlatform
{
    public String platformName;
    public List<String> preprocessorDefines = new List<string>();
    public List<String> linkerLibraries = new List<string>();
}


public abstract class IToolchain
{
    private static List<IToolchain> _toolchains = null;

    public static List<IToolchain> GetToolchains()
    {
        if (_toolchains == null)
        {
            _toolchains = new List<IToolchain>();

            var types = typeof(IToolchain).Assembly.GetTypes().Where(t => t.IsSubclassOf(typeof(IToolchain)));
            foreach (var type in types)
            {
                if (!type.IsAbstract)
                {
                    var instanceProperty = type.GetProperty("Instance",
                        BindingFlags.Public | BindingFlags.Static | BindingFlags.FlattenHierarchy);
                    if (instanceProperty != null)
                    {

                        try
                        {
                            IToolchain toolchain = (IToolchain) instanceProperty.GetValue(null, null);
                            _toolchains.Add(toolchain);
                        }
                        catch (Exception)
                        {
                        }
                    }
                }
            }
        }

        return _toolchains;
    }

    protected static int ExecuteCommand(string command, out string output)
    {
        Process process = new Process();
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.RedirectStandardOutput = true;
        process.StartInfo.RedirectStandardError = true;
        process.StartInfo.FileName = "cmd.exe";
        process.StartInfo.Arguments = "/C" + command;
        process.Start();

        output = process.StandardOutput.ReadToEnd();
        output += process.StandardError.ReadToEnd();
        process.WaitForExit();

        return process.ExitCode;
    }

    protected static int ExecuteCommand(string command)
    {
        string discardLog = "";
        return ExecuteCommand(command, out discardLog);
    }



    public abstract List<BuildPlatform> GetPlatforms();

    public abstract bool PrepareModuleForBuild(ModuleManifest manifest, BuildPlatform target);

    public abstract bool BuildSource(string sourceFile, List<string> includePaths, List<string> preprocessorDefines);

    public abstract bool LinkLibrary(List<string> sourceFiles);

    public abstract bool LinkExecutable(ProjectManifest project, List<ModuleManifest> modules);
}

public abstract class IToolchainInterface<T> : IToolchain where T : IToolchain, new()
{
    public static T Instance
    {
        get
        {
            if (_instance == null)
            {
                _instance = new T();
            }

            return _instance;
        }
    }

    private static T _instance = null;
}
