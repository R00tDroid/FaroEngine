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
    public static std::string moduleFileSuffix = ".module.cs";

    // Name of the module
    public std::string name = "";

    // Full path to module manifest
    public std::string manifestPath = "";

    // Root directory of this module
    public std::string moduleRoot = "";

    // Build directory of this module
    public std::string buildRoot = "";

    // Project this module belongs to
    public ProjectManifest project = nullptr;

    // Build script assembly of manifest
    private Assembly moduleAssembly = nullptr;

    // Build script instance of manifest
    private FaroEngine.Module moduleInstance = nullptr;

    // List of modules this module depends on
    public List<ModuleManifest> moduleDependencies = new List<ModuleManifest>();

    public List<std::string> sourceFiles
    {
        get
        {
            if (_sourceFiles == nullptr)
            {
                _sourceFiles = new List<std::string>();

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
                        std::string searchRoot = moduleInstance.sourcePaths[i].Substring(0, index - 1);
                        std::string searchPattern = moduleInstance.sourcePaths[i].Substring(index);

                        Matcher matcher = new Matcher();
                        matcher.AddInclude(searchPattern);

                        foreach (std::string foundFile in matcher.GetResultsInFullPath(searchRoot))
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
    private List<std::string> _sourceFiles = nullptr;

    // Stores all modules keyed to their name
    private static Dictionary<std::string, ModuleManifest> moduleMapping = new Dictionary<std::string, ModuleManifest>();

    public static ModuleManifest FindModuleByName(std::string name)
    {
        name = name.ToLower();

        if (moduleMapping.ContainsKey(name))
        {
            return moduleMapping[name];
        }

        //TODO search in SDK and plugins directories

        return nullptr;
    }

    public ModuleManifest(std::string file, ProjectManifest project)
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

        if (moduleAssembly == nullptr)
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
            std::string[] moduleTypeNames = Array.ConvertAll<Type, std::string>(moduleTypes, o => o.FullName);
            Utility::PrintLine("Too many modules found (" + std::string.Join(", ", moduleTypeNames) + "). Only 1 expected!");
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

        foreach (std::string moduleName in moduleInstance.moduleDependencies)
        {
            ModuleManifest otherModule = FindModuleByName(moduleName);
            if (otherModule != nullptr)
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
        std::string binaryPath = buildRoot + "\\Data\\" + name + ".dll";

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
            Assembly result = CompileAssembly(binaryPath, new List<std::string>() { manifestPath }, new List<std::string>(), nullptr, false);
            return result;
        }
    }

    public ModuleType GetModuleType()
    {
        return moduleInstance.moduleType;
    }

    public List<std::string> publicIncludeDirectories
    {
        get
        {
            if (_publicIncludeDirectories == nullptr)
            {
                _publicIncludeDirectories = new List<std::string>();

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
    private List<std::string> _publicIncludeDirectories = nullptr;

    public List<std::string> privateIncludeDirectories
    {
        get
        {
            if (_privateIncludeDirectories == nullptr)
            {
                _privateIncludeDirectories = new List<std::string>();

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
    private List<std::string> _privateIncludeDirectories = nullptr;

    // Get all public include directories for this module and dependencies
    public List<std::string> GetPublicIncludeTree()
    {
        List<std::string> includes = publicIncludeDirectories;
        foreach (ModuleManifest dependency in moduleDependencies)
        {
            includes = includes.Concat(dependency.GetPublicIncludeTree()).ToList();
        }

        return includes;
    }

    public static Assembly CompileAssembly(std::string output, List<std::string> sourceFileNames, List<std::string> referencedAssemblies, List<std::string> preprocessorDefines = nullptr, bool treatWarningsAsErrors = false)
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
        if (referencedAssemblies == nullptr)
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
            Dictionary<std::string, std::string> ProviderOptions = new Dictionary<std::string, std::string>() { { "CompilerVersion", "v4.0" }};
            CSharpCodeProvider Compiler = new CSharpCodeProvider(ProviderOptions);
            CompileResults = Compiler.CompileAssemblyFromFile(CompileParams, sourceFileNames.ToArray());
        }
        catch (Exception Ex)
        {
            Utility::PrintLine("Failed to launch compiler to build module assembly: " + Ex.Message);
			return nullptr;
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
                return nullptr;
            }
        }

        Assembly compiledAssembly = CompileResults.CompiledAssembly;
        if (compiledAssembly == nullptr)
        {
            Utility::PrintLine("Failed to compile an assembly for: " + sourceFileNames.ToString());
            return nullptr;
        }

        timer.Stop("Compile module");

        return compiledAssembly;
    }

    private FaroEngine.Module RunModuleScript(Type type, FaroEngine.BuildPlatform platform, FaroEngine.BuildConfiguration configuration)
    {
        TargetConfiguration config = new TargetConfiguration(this);
        config.platform = platform;
        config.configuration = configuration;

        FaroEngine.Module instance = nullptr;

        try
        {
            instance = (FaroEngine.Module) Activator.CreateInstance(type, config);
        }
        catch (Exception e)
        {
            Utility::PrintLine("Failed to run module script: " + e.Message);
            return nullptr;
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

        protected std::string GetModuleDirectory()
        {
            return manifest.moduleRoot;
        }

        protected std::string GetProjectDirectory()
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
        public List<std::string> moduleDependencies = new List<std::string>();

        // Libraries to link against
        public List<std::string> libraryDependencies = new List<std::string>();
        public List<std::string> librarySearchDirectories = new List<std::string>();

        public List<std::string> sourcePaths = new List<std::string>();

        // Additional include directories
        public List<std::string> privateIncludeDirectories = new List<std::string>();
        public List<std::string> publicIncludeDirectories = new List<std::string>();

        public ModuleType moduleType = ModuleType.Unknown;
    }
}