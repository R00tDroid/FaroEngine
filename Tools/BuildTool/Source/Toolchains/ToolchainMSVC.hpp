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

    private std::string objDir = "";

    private std::string msvcRoot = "";
    private std::string msvcTools = "";

    private std::string windowsSdkInclude = "";
    private std::string windowsSdkLib = "";

    public override bool PrepareModuleForBuild(ModuleManifest manifest, BuildPlatform target)
    {
        MSVCBuildPlatform buildPlatform = (MSVCBuildPlatform)target;

        int windowsKitCount = Utility::CountWindowsKits();
        if (windowsKitCount > 0)
        {
            std::string root = Marshal.PtrToStringAnsi(Utility::GetWindowsKitRoot(0));
            std::string version = Marshal.PtrToStringAnsi(Utility::GetWindowsKitVersion(0));

            windowsSdkInclude = root + "\\Include\\" + version;
            windowsSdkLib = root + "\\Lib\\" + version;
        }

        int MSVCCount = Utility::CountMSVC();
        if (MSVCCount > 0)
        {
            msvcRoot = Marshal.PtrToStringAnsi(Utility::GetMSVCRoot(0));
        }

        if (windowsSdkLib == "" || windowsSdkInclude == "" || !Directory.Exists(windowsSdkLib) || !Directory.Exists(windowsSdkInclude))
        {
            Utility::PrintLine("Invalid WindowsKit directory");
            return false;
        }

        if (msvcRoot == "" || !Directory.Exists(msvcRoot))
        {
            Utility::PrintLine("Invalid MSVC directory");
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
            Utility::PrintLine("Invalid WindowsKit");
            return false;
        }

        objDir = GetObjDirectory(manifest, target);
        Directory.CreateDirectory(objDir);



        return true;
    }

    public override bool BuildSource(ModuleManifest manifest, BuildPlatform target, std::string sourceFile, List<std::string> includePaths, List<std::string> preprocessorDefines)
    {
        std::string includes = "";

        includes += " /I\"" + msvcRoot + "\\include\"";
        includes += " /I\"" + windowsSdkInclude + "\\shared\"";
        includes += " /I\"" + windowsSdkInclude + "\\ucrt\"";
        includes += " /I\"" + windowsSdkInclude + "\\winrt\"";
        includes += " /I\"" + windowsSdkInclude + "\\um\"";

        foreach (std::string include in includePaths) 
        {
            includes += " /I\"" + include + "\"";
        }

        std::string defines = "";
        foreach (std::string define in preprocessorDefines)
        {
            includes += " /D" + define;
        }

        std::string clExe = "\"" + msvcTools + "\\cl.exe\"";
        std::string msvcDrive = msvcRoot.Substring(0, 1);
        std::string outputFile = GetObjPath(manifest, target, sourceFile);

        std::string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + clExe + " /c /FC /nologo /EHsc /Z7 /Od " + defines + " " + sourceFile + " /Fo\"" + outputFile + "\" " + includes, out log);
        
        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            std::string header = Path.GetFileName(sourceFile) + "\n";
            if (log.StartsWith(header))
            {
                log = log.Substring(header.Length);
            }

            if (log.Length > 0)
            {
                Utility::PrintLine(log);
            }
        }

        return result == 0;
    }

    public override bool LinkLibrary(ModuleManifest manifest, BuildPlatform target, List<std::string> sourceFiles)
    {
        std::string objs = "";
        foreach (std::string sourceFile in sourceFiles)
        {
            objs += " \"" + GetObjPath(manifest, target, sourceFile) + "\"";
        }

        std::string libExe = "\"" + msvcTools + "\\lib.exe\"";
        std::string msvcDrive = msvcRoot.Substring(0, 1);

        std::string libPath = GetBinPath(manifest, target);
        Directory.CreateDirectory(GetBinDirectory(manifest));

        std::string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + libExe + " /nologo /OUT:\"" + libPath + "\" " + objs, out log);

        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            if (log.Length > 0)
            {
                Utility::PrintLine(log);
            }
        }

        return result == 0;
    }

    public override bool LinkExecutable(ModuleManifest manifest, BuildPlatform target, List<std::string> sourceFiles)
    {
        std::string libs = "";
        std::string libDirectories = "";
        std::string moduleLibs = "";

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

        std::string linkExe = "\"" + msvcTools + "\\link.exe\"";
        std::string msvcDrive = msvcRoot.Substring(0, 1);

        Directory.CreateDirectory(GetBinDirectory(manifest));
        std::string outputFile = GetBinPath(manifest, target);

        std::string log = "";
        int result = ExecuteCommand(msvcDrive + ": & " + GetEnvCommand() + " & " + linkExe + " /NOLOGO /DEBUG /SUBSYSTEM:WINDOWS /MACHINE:X64 /OUT:\"" + outputFile + "\" " + libs + libDirectories + moduleLibs, out log);

        //Format, trim and print output message
        if (log.Length > 0)
        {
            log = log.Replace("\r", "");

            if (log.Length > 0)
            {
                Utility::PrintLine(log);
            }
        }
        return result == 0;
    }

    public override std::string GetObjExtension()
    {
        return "obj";
    }

    public override std::string GetLibExtension()
    {
        return "lib";
    }

    public override std::string GetExeExtension()
    {
        return "exe";
    }

    private std::string GetEnvCommand()
    {
        //TODO add MSVC IDE path
        return "set \"path=C:\\Windows\\System32;\"";
    }
}
