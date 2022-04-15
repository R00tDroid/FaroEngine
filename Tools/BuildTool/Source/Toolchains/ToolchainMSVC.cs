using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

enum EMSVCArchetecture 
{
    x64,
    x86
};

class MSVCBuildPlatform : BuildPlatform 
{
    public EMSVCArchetecture Architecture;
}

public class ToolchainMSVC : IToolchainInterface<ToolchainMSVC>
{
    public override List<BuildPlatform> GetPlatforms()
    {
        return new List<BuildPlatform>()
        {
            new MSVCBuildPlatform() { Architecture = EMSVCArchetecture.x64, platformName = "Windows x64", preprocessorDefines = { "FARO_OS_WINDOWS", "FARO_ARCH_X64" }, linkerLibraries = {} },
            new MSVCBuildPlatform() { Architecture = EMSVCArchetecture.x86, platformName = "Windows x86", preprocessorDefines = { "FARO_OS_WINDOWS", "FARO_ARCH_X86" }, linkerLibraries = {} }
        };
    }

    private string objDir = "";
    private string libPath = "";

    private string msvcRoot = "";
    private string msvcTools = "";

    private string windowsSdkInclude = "";
    private string windowsSdkLib = "";

    public override bool PrepareModuleForBuild(ModuleManifest manifest, BuildPlatform target)
    {
        MSVCBuildPlatform buildPlatform = (MSVCBuildPlatform)target;

        int windowsKitCount = Utility.CountWindowsKits();
        if (windowsKitCount > 0)
        {
            string root = Marshal.PtrToStringAnsi(Utility.GetWindowsKitRoot(0));
            string version = Marshal.PtrToStringAnsi(Utility.GetWindowsKitVersion(0));

            windowsSdkInclude = root + "\\Include\\" + version;
            windowsSdkLib = root + "\\Lib\\" + version;
        }

        int MSVCCount = Utility.CountMSVC();
        if (MSVCCount > 0)
        {
            msvcRoot = Marshal.PtrToStringAnsi(Utility.GetMSVCRoot(0));
        }

        if (windowsSdkLib == "" || windowsSdkInclude == "" || !Directory.Exists(windowsSdkLib) || !Directory.Exists(windowsSdkInclude))
        {
            Utility.PrintLine("Invalid WindowsKit directory");
            return false;
        }

        if (msvcRoot == "" || !Directory.Exists(msvcRoot))
        {
            Utility.PrintLine("Invalid MSVC directory");
            return false;
        }

        msvcTools = msvcRoot + "\\bin\\";
        switch (buildPlatform.Architecture)
        {
            case EMSVCArchetecture.x64:
                {
                    msvcTools += "Hostx64\\x64";
                    break;
                }
            case EMSVCArchetecture.x86:
                {
                    msvcTools += "Hostx86\\x86";
                    break;
                }
        }

        if (!Directory.Exists(windowsSdkInclude) || !Directory.Exists(windowsSdkLib) || !Directory.Exists(msvcTools))
        {
            Utility.PrintLine("Invalid WindowsKit");
            return false;
        }

        objDir = manifest.buildRoot + "\\Obj\\";
        Directory.CreateDirectory(objDir);

        libPath = GetModuleLibrary(manifest);
        Directory.CreateDirectory(GetLibDirForModule(manifest));

        return true;
    }

    private string GetObjForSource(string file)
    {
        return objDir + "\\" + Path.GetFileNameWithoutExtension(file) + ".obj";
    }

    public override bool BuildSource(string sourceFile, List<string> includePaths, List<string> preprocessorDefines)
    {
        string includes = "";

        includes += " /I\"" + msvcRoot + "\\include\"";
        includes += " /I\"" + windowsSdkInclude + "\\shared\"";
        includes += " /I\"" + windowsSdkInclude + "\\ucrt\"";
        includes += " /I\"" + windowsSdkInclude + "\\winrt\"";
        includes += " /I\"" + windowsSdkInclude + "\\um\"";

        foreach (string include in includePaths) 
        {
            includes += " /I\"" + include + "\"";
        }

        string defines = "";
        foreach (string define in preprocessorDefines)
        {
            includes += " /D" + define;
        }

        string clExe = "\"" + msvcTools + "\\cl.exe\"";
        string msvcDrive = msvcRoot.Substring(0, 1);
        string outputFile = GetObjForSource(sourceFile);

        string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + clExe + " /c /FC /nologo /EHsc /Z7 /Od " + defines + " " + sourceFile + " /Fo\"" + outputFile + "\" " + includes, out log);
        
        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            string header = Path.GetFileName(sourceFile) + "\n";
            if (log.StartsWith(header))
            {
                log = log.Substring(header.Length);
            }

            if (log.Length > 0)
            {
                Utility.PrintLine(log);
            }
        }

        return result == 0;
    }

    public override bool LinkLibrary(ModuleManifest module)
    {
        string objs = "";
        foreach (string sourceFile in module.sourceFiles)
        {
            objs += " \"" + GetObjForSource(sourceFile) + "\"";
        }

        string libExe = "\"" + msvcTools + "\\lib.exe\"";
        string msvcDrive = msvcRoot.Substring(0, 1);

        string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + libExe + " /nologo /OUT:\"" + libPath + "\" " + objs, out log);

        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            if (log.Length > 0)
            {
                Utility.PrintLine(log);
            }
        }

        return result == 0;
    }

    public override bool LinkExecutable(ModuleManifest module)
    {
        string libs = "";
        string libDirectories = "";
        string moduleLibs = "";

        libs += " \"user32.lib\"";

        //TODO switch debug and release
        libs += " \"libucrtd.lib\"";
        libs += " \"libvcruntimed.lib\"";

        //TODO Set correct architecture
        libDirectories += " /LIBPATH:\"" + windowsSdkLib + "\\ucrt\\x64\"";
        libDirectories += " /LIBPATH:\"" + windowsSdkLib + "\\um\\x64\"";
        libDirectories += " /LIBPATH:\"" + msvcRoot + "\\lib\\x64\"";

        //TODO link against other modules (/WHOLEARCHIVE)
        //TODO link against dependencies

        string linkExe = "\"" + msvcTools + "\\link.exe\"";
        string msvcDrive = msvcRoot.Substring(0, 1);

        string outDir = module.buildRoot + "\\bin";
        Directory.CreateDirectory(outDir);
        string outputFile = outDir + "\\" + module.name + ".exe";

        string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + linkExe + " /NOLOGO /DEBUG /SUBSYSTEM:WINDOWS /MACHINE:X64 /OUT:\"" + outputFile + "\" " + libs + libDirectories + moduleLibs, out log);

        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            if (log.Length > 0)
            {
                Utility.PrintLine(log);
            }
        }
        return result == 0;
    }

    private string GetEnvCommand()
    {
        //TODO add MSVC IDE path
        return "set \"path=C:\\Windows\\System32;\"";
    }

    private string GetLibDirForModule(ModuleManifest manifest)
    {
        return manifest.buildRoot + "\\lib";
    }

    private string GetModuleLibrary(ModuleManifest manifest)
    {
        return GetLibDirForModule(manifest) + "\\Windows.lib";
    }
}
