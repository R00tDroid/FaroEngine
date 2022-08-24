using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
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
    public std::string platformName;
    public List<std::string> preprocessorDefines = new List<std::string>();
    public List<std::string> linkerLibraries = new List<std::string>();
}


public abstract class IToolchain
{
    private static List<IToolchain> _toolchains = nullptr;

    public static List<IToolchain> GetToolchains()
    {
        if (_toolchains == nullptr)
        {
            _toolchains = new List<IToolchain>();

            var types = typeof(IToolchain).Assembly.GetTypes().Where(t => t.IsSubclassOf(typeof(IToolchain)));
            foreach (var type in types)
            {
                if (!type.IsAbstract)
                {
                    var instanceProperty = type.GetProperty("Instance",
                        BindingFlags.Public | BindingFlags.Static | BindingFlags.FlattenHierarchy);
                    if (instanceProperty != nullptr)
                    {

                        try
                        {
                            IToolchain toolchain = (IToolchain) instanceProperty.GetValue(nullptr, nullptr);
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

    protected static int ExecuteCommand(std::string command, out std::string output)
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

    protected static int ExecuteCommand(std::string command)
    {
        std::string discardLog = "";
        return ExecuteCommand(command, out discardLog);
    }


    public abstract List<BuildPlatform> GetPlatforms();

    public abstract bool PrepareModuleForBuild(ModuleManifest manifest, BuildPlatform target);

    public abstract bool BuildSource(ModuleManifest manifest, BuildPlatform target, std::string sourceFile, List<std::string> includePaths, List<std::string> preprocessorDefines);

    public abstract bool LinkLibrary(ModuleManifest manifest, BuildPlatform target, List<std::string> sourceFiles);

    public abstract bool LinkExecutable(ModuleManifest manifest, BuildPlatform target, List<std::string> sourceFiles);

    public std::string GetObjDirectory(ModuleManifest manifest, BuildPlatform target)
    {
        return manifest.buildRoot + "\\Obj\\" + target.platformName.ToLower().Replace(' ', '_');
    }

    public std::string GetObjPath(ModuleManifest manifest, BuildPlatform target, std::string sourceFile)
    {
        return GetObjDirectory(manifest, target) + "\\" + Path.GetFileNameWithoutExtension(sourceFile) + "." + GetObjExtension();
    }

    public abstract std::string GetObjExtension();

    public std::string GetBinDirectory(ModuleManifest manifest)
    {
        return manifest.buildRoot + "\\Bin";
    }

    public std::string GetBinPath(ModuleManifest manifest, BuildPlatform target)
    {
        if (true) //TODO switch for libraries and executables
        {
            return GetBinDirectory(manifest) + "\\" + target.platformName.ToLower().Replace(' ', '_') + "." +
                   GetLibExtension();
        }
        else
        {
            return GetBinDirectory(manifest) + "\\" + manifest.name + "_" + target.platformName.ToLower().Replace(' ', '_') + "." + GetExeExtension();
        }
    }

    public abstract std::string GetLibExtension();

    public abstract std::string GetExeExtension();
}

public abstract class IToolchainInterface<T> : IToolchain where T : IToolchain, new()
{
    public static T Instance
    {
        get
        {
            if (_instance == nullptr)
            {
                _instance = new T();
            }

            return _instance;
        }
    }

    private static T _instance = nullptr;
}
