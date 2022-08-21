using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Remoting;
using Microsoft.CSharp;
using FaroEngine;
using Microsoft.Extensions.FileSystemGlobbing;

public class ModuleManifest
{
    public static String moduleFileSuffix = ".module.cs";

    // Name of the module
    public String name = "";

    // Full path to module manifest
    public String manifestPath = "";

    // Root directory of this module
    public String moduleRoot = "";

    // Build directory of this module
    public String buildRoot = "";

    // Project this module belongs to
    public ProjectManifest project = null;

    // Build script assembly of manifest
    private Assembly moduleAssembly = null;

    // Build script instance of manifest
    private FaroEngine.Module moduleInstance = null;

    // List of modules this module depends on
    public List<ModuleManifest> moduleDependencies = new List<ModuleManifest>();

    public List<string> sourceFiles
    {
        get
        {
            if (_sourceFiles == null)
            {
                _sourceFiles = new List<string>();

                for (int i = 0; i < moduleInstance.sourcePaths.Count; i++)
                {
                    // Ensure path is absolute
                    if (!Path.IsPathRooted(moduleInstance.sourcePaths[i]))
                    {
                        moduleInstance.sourcePaths[i] = moduleRoot + "\\" + moduleInstance.sourcePaths[i];
                    }

                    if (!moduleInstance.sourcePaths[i].Contains('*'))
                    {
                        _sourceFiles.Add(moduleInstance.sourcePaths[i]);
                    }
                    else
                    {
                        int index = moduleInstance.sourcePaths[i].IndexOf('*');
                        string searchRoot = moduleInstance.sourcePaths[i].Substring(0, index - 1);
                        string searchPattern = moduleInstance.sourcePaths[i].Substring(index);

                        Matcher matcher = new Matcher();
                        matcher.AddInclude(searchPattern);

                        foreach (string foundFile in matcher.GetResultsInFullPath(searchRoot))
                        {
                            _sourceFiles.Add(foundFile);
                        }
                    }
                }

                _sourceFiles.Sort();
            }
            return _sourceFiles;
        }
    }
    private List<string> _sourceFiles = null;

    // Stores all modules keyed to their name
    private static Dictionary<string, ModuleManifest> moduleMapping = new Dictionary<string, ModuleManifest>();

    public static ModuleManifest FindModuleByName(string name)
    {
        name = name.ToLower();

        if (moduleMapping.ContainsKey(name))
        {
            return moduleMapping[name];
        }

        //TODO search in SDK and plugins directories

        return null;
    }

    public ModuleManifest(String file, ProjectManifest project)
    {
        manifestPath = file;
        moduleRoot = Directory.GetParent(manifestPath).FullName;
        buildRoot = moduleRoot + "\\.Faro";
        this.project = project;
        name = Path.GetFileName(manifestPath).Replace(moduleFileSuffix, "");

        if (!Directory.Exists(buildRoot))
        {
            DirectoryInfo dirInfo = Directory.CreateDirectory(buildRoot);
            dirInfo.Attributes |= FileAttributes.Hidden;
        }
    }

    // Parse the build script and register this new module
    public bool LoadModule()
    {
        PerformanceTimer timer = new PerformanceTimer();

        Utility::PrintLineD("Loading module assembly");
        moduleAssembly = GetAssembly();

        if (moduleAssembly == null)
        {
            return false;
        }

        PerformanceTimer loadTimer = new PerformanceTimer();
        Utility::PrintLineD("Loading module into domain");
        try
        {
            AppDomain.CurrentDomain.Load(moduleAssembly.GetName());
        }
        catch (Exception Ex)
        {
            Utility::PrintLine("Unable to load module assembly: " + Ex.Message);
            return false;
        }
        loadTimer.Stop("Load assembly");

        PerformanceTimer reflectTimer = new PerformanceTimer();
        Utility::PrintLineD("Loading module instance");
        Type[] typesInAssembly = moduleAssembly.GetTypes();
        Type[] moduleTypes = typesInAssembly.Where(c => c.IsSubclassOf(typeof(FaroEngine.Module))).ToArray();
        reflectTimer.Stop("Reflection scan");

        PerformanceTimer executeTimer = new PerformanceTimer();
        if (moduleTypes.Length == 0)
        {
            Utility::PrintLine("No module found");
            return false;
        }
        else if (moduleTypes.Length > 1)
        {
            string[] moduleTypeNames = Array.ConvertAll<Type, string>(moduleTypes, o => o.FullName);
            Utility::PrintLine("Too many modules found (" + String.Join(", ", moduleTypeNames) + "). Only 1 expected!");
            return false;

        }
        else
        {
            moduleInstance = RunModuleScript(moduleTypes[0], FaroEngine.BuildPlatform.EditorWindows, FaroEngine.BuildConfiguration.Debug);
        }
        executeTimer.Stop("Run script");

        moduleMapping.Add(name.ToLower(), this);

        timer.Stop("Load module: " + name);

        return true;
    }

    // Resolve all module dependencies from the names provided by build script
    public bool ResolveDependencies()
    {
        moduleDependencies.Clear();

        bool failed = false;

        foreach (string moduleName in moduleInstance.moduleDependencies)
        {
            ModuleManifest otherModule = FindModuleByName(moduleName);
            if (otherModule != null)
            {
                moduleDependencies.Add(otherModule);
            }
            else
            {
                Utility::PrintLine("Unable to find module dependency: " + moduleName);
                failed = true;
            }
        }

        return !failed;
    }

    // Load or compile the build script assembly
    public Assembly GetAssembly()
    {
        String binaryPath = buildRoot + "\\Data\\" + name + ".dll";

        if (File.Exists(binaryPath) && false) //TODO check for changes
        {
            PerformanceTimer timer = new PerformanceTimer();
            Utility::PrintLineD("Loading prebuild module assembly: " + binaryPath);
            Assembly result = Assembly.LoadFile(binaryPath);
            timer.Stop("Load module binary");
            return result;
        }
        else
        {
            Assembly result = CompileAssembly(binaryPath, new List<string>() { manifestPath }, new List<string>(), null, false);
            return result;
        }
    }

    public ModuleType GetModuleType()
    {
        return moduleInstance.moduleType;
    }

    public List<string> publicIncludeDirectories
    {
        get
        {
            if (_publicIncludeDirectories == null)
            {
                _publicIncludeDirectories = new List<string>();

                for (int i = 0; i < moduleInstance.publicIncludeDirectories.Count; i++)
                {
                    // Ensure path is absolute
                    if (!Path.IsPathRooted(moduleInstance.publicIncludeDirectories[i]))
                    {
                        moduleInstance.publicIncludeDirectories[i] = moduleRoot + "\\" + moduleInstance.publicIncludeDirectories[i];
                    }

                    _publicIncludeDirectories.Add(moduleInstance.publicIncludeDirectories[i]);
                }
            }
            return _publicIncludeDirectories;
        }
    }
    private List<string> _publicIncludeDirectories = null;

    public List<string> privateIncludeDirectories
    {
        get
        {
            if (_privateIncludeDirectories == null)
            {
                _privateIncludeDirectories = new List<string>();

                for (int i = 0; i < moduleInstance.privateIncludeDirectories.Count; i++)
                {
                    // Ensure path is absolute
                    if (!Path.IsPathRooted(moduleInstance.publicIncludeDirectories[i]))
                    {
                        moduleInstance.privateIncludeDirectories[i] = moduleRoot + "\\" + moduleInstance.privateIncludeDirectories[i];
                    }

                    _privateIncludeDirectories.Add(moduleInstance.privateIncludeDirectories[i]);
                }
            }
            return _privateIncludeDirectories;
        }
    }
    private List<string> _privateIncludeDirectories = null;

    // Get all public include directories for this module and dependencies
    public List<string> GetPublicIncludeTree()
    {
        List<string> includes = publicIncludeDirectories;
        foreach (ModuleManifest dependency in moduleDependencies)
        {
            includes = includes.Concat(dependency.GetPublicIncludeTree()).ToList();
        }

        return includes;
    }

    public static Assembly CompileAssembly(String output, List<String> sourceFileNames, List<string> referencedAssemblies, List<string> preprocessorDefines = null, bool treatWarningsAsErrors = false)
    {
        PerformanceTimer timer = new PerformanceTimer();

        Utility::PrintLineD("Preparing assembly compilation");

		TempFileCollection temporaryFiles = new TempFileCollection();

		// Setup compile parameters
		CompilerParameters CompileParams = new CompilerParameters();
        
		CompileParams.GenerateInMemory = false;
        CompileParams.OutputAssembly = output;
        CompileParams.GenerateExecutable = false;
        CompileParams.TempFiles = temporaryFiles;

        CompileParams.TreatWarningsAsErrors = false;
        CompileParams.WarningLevel = 4;
        CompileParams.TreatWarningsAsErrors = treatWarningsAsErrors;

        CompileParams.IncludeDebugInformation = false;

        Utility::PrintLineD("Compiling " + output);

        // Add assembly references
        if (referencedAssemblies == null)
		{
			// Always depend on the CLR System assembly
			CompileParams.ReferencedAssemblies.Add("System.dll");
			CompileParams.ReferencedAssemblies.Add("System.Core.dll");
		}
		else
		{
			// Add in the set of passed in referenced assemblies
			CompileParams.ReferencedAssemblies.AddRange(referencedAssemblies.ToArray());
		}

        Assembly interfaceAssembly = Assembly.GetAssembly(typeof(FaroEngine.Module));
        CompileParams.ReferencedAssemblies.Add(interfaceAssembly.Location);

        CompileParams.CompilerOptions = "-preferreduilang:en-US";

        Utility::PrintLineD("Creating assembly output directory");
        if (!Directory.Exists(Directory.GetParent(output).FullName))
        {
            Directory.CreateDirectory(Directory.GetParent(output).FullName);
        }

        Utility::PrintLineD("Compiling assembly");
        CompilerResults CompileResults;
        try
        {
            Dictionary<string, string> ProviderOptions = new Dictionary<string, string>() { { "CompilerVersion", "v4.0" }};
            CSharpCodeProvider Compiler = new CSharpCodeProvider(ProviderOptions);
            CompileResults = Compiler.CompileAssemblyFromFile(CompileParams, sourceFileNames.ToArray());
        }
        catch (Exception Ex)
        {
            Utility::PrintLine("Failed to launch compiler to build module assembly: " + Ex.Message);
			return null;
        }

        if (CompileResults.Errors.Count > 0)
        {
            Utility::PrintLine("Module compilation:");
            foreach (CompilerError CurError in CompileResults.Errors)
            {
                Utility::PrintLine("\t" + CurError.ToString());
            }
            if (CompileResults.Errors.HasErrors || treatWarningsAsErrors)
            {
                Utility::PrintLine("Unable to compile");
                return null;
            }
        }

        Assembly compiledAssembly = CompileResults.CompiledAssembly;
        if (compiledAssembly == null)
        {
            Utility::PrintLine("Failed to compile an assembly for: " + sourceFileNames.ToString());
            return null;
        }

        timer.Stop("Compile module");

        return compiledAssembly;
    }

    private FaroEngine.Module RunModuleScript(Type type, FaroEngine.BuildPlatform platform, FaroEngine.BuildConfiguration configuration)
    {
        TargetConfiguration config = new TargetConfiguration(this);
        config.platform = platform;
        config.configuration = configuration;

        FaroEngine.Module instance = null;

        try
        {
            instance = (FaroEngine.Module) Activator.CreateInstance(type, config);
        }
        catch (Exception e)
        {
            Utility::PrintLine("Failed to run module script: " + e.Message);
            return null;
        }

        return instance;
    }
}

namespace FaroEngine
{
    public enum BuildPlatform
    {
        EditorWindows,
        Windows,
        Android,
        Web
    }

    public enum BuildConfiguration
    {
        Debug,
        Development,
        Shipping
    }

    public enum ModuleType
    {
        Unknown,
        Library,
        Executable
    }


    public class TargetConfiguration
    {
        public TargetConfiguration(ModuleManifest manifest)
        {
            this.manifest = manifest;
        }
        private ModuleManifest manifest;

        public BuildPlatform platform;
        public BuildConfiguration configuration;

        protected String GetModuleDirectory()
        {
            return manifest.moduleRoot;
        }

        protected String GetProjectDirectory()
        {
            return manifest.project.projectDirectory;
        }
    }

    public abstract class Module
    {
        public Module(TargetConfiguration config)
        {
        }

        // Module interface
        public List<String> moduleDependencies = new List<String>();

        // Libraries to link against
        public List<String> libraryDependencies = new List<String>();
        public List<String> librarySearchDirectories = new List<String>();

        public List<String> sourcePaths = new List<String>();

        // Additional include directories
        public List<String> privateIncludeDirectories = new List<String>();
        public List<String> publicIncludeDirectories = new List<String>();

        public ModuleType moduleType = ModuleType.Unknown;
    }
}