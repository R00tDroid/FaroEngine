using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32;

public static class Utility
{
    [DllImport("ToolchainInfo.dll")]
    public static extern int CountWindowsKits();

    [DllImport("ToolchainInfo.dll")]
    public static extern IntPtr GetWindowsKitRoot(int Index);

    [DllImport("ToolchainInfo.dll")]
    public static extern IntPtr GetWindowsKitVersion(int Index);

    [DllImport("ToolchainInfo.dll")]
    public static extern int CountMSVC();

    [DllImport("ToolchainInfo.dll")]
    public static extern IntPtr GetMSVCRoot(int Index);

    public static void PrintLine(String log)
    {
        Print(log + "\n");
    }

    public static void Print(String log)
    {
        Console.Write(log);
        Trace.Write(log);
    }

    public static void PrintLineD(String log)
    {
#if DEBUG
        PrintD(log + "\n");
#endif
    }

    public static void PrintD(String log)
    {
#if DEBUG
        Print("[D] " + log);
#endif
    }
}

class PerformanceTimer
{
    private static Stopwatch applicationTimer = null;

    public static void StartGlobalTimer()
    {
        applicationTimer = Stopwatch.StartNew();
    }

    public static float GetMillisSinceStart()
    {
        if (applicationTimer == null)
        {
            return 0.0f;
        }
        else
        {
            long microseconds = applicationTimer.ElapsedTicks / (Stopwatch.Frequency / (1000L * 1000L));
            return microseconds / 1000.0f;
        }
    }


    private static int globalDepth = 0;
    private static int timerCount = 0;
    private int depth = 0;
    private int index = 0;
    private Stopwatch timer = null;

    private static SortedDictionary<int, string> timerReports = new SortedDictionary<int, string>();

    public PerformanceTimer()
    {
        timer = Stopwatch.StartNew();

        depth = globalDepth;
        globalDepth++;

        index = timerCount;
        timerCount++;
    }

    ~PerformanceTimer()
    {
        if (timer != null) 
        {
            globalDepth--;
            timer = null;
        }
    }

    public void Stop(string label)
    {
        if (timer != null)
        {
            globalDepth--;

            timer.Stop();
            long microseconds = timer.ElapsedTicks / (Stopwatch.Frequency / (1000L * 1000L));
            string message = "";
            for (int i = 0; i < depth; i++) 
            {
                message += " |";
            }
            message += "-*>";
            message += "[" + label + "] " + (microseconds / 1000.0f);

            timerReports.Add(index, message);

            timer = null;
        }
    }

    public static void PrintTimers() 
    {
        Utility.PrintLine("\n--Perf report--");
        foreach (var report in timerReports.Values) 
        {
            Utility.PrintLine(report);
        }
    }
}

/*public class GUIDManager
{
    public GUIDManager(ProjectManifest project)
    {
        this.project = project;
        Load();
    }

    ~GUIDManager()
    {
        if (anyChanges)
        {
            Save();
        }
    }

    public void Load()
    {
        GUIDs.Clear();

        String database = GetDatabasePath();
        if (File.Exists(database))
        {
            BinaryReader reader = new BinaryReader(File.OpenRead(GetDatabasePath()));
            int count = reader.ReadInt32();

            for (int i = 0; i < count; i++)
            {
                string key = reader.ReadString();
                string value = reader.ReadString();
                GUIDs.Add(key, value);
            }

            reader.Close();
        }
    }

    public void Save()
    {
        BinaryWriter writer = new BinaryWriter(File.Open(GetDatabasePath(), FileMode.Create));
        writer.Write(GUIDs.Count);
        foreach (var keyPair in GUIDs)
        {
            writer.Write(keyPair.Key);
            writer.Write(keyPair.Value);
        }
        writer.Close();
    }

    public string GetGUID(string id)
    {
        if (!GUIDs.ContainsKey(id))
        {
            GUIDs.Add(id, Guid.NewGuid().ToString().ToUpper());
            anyChanges = true;
        }

        return GUIDs[id];
    }
    private Dictionary<string, string> GUIDs = new Dictionary<string, string>();
    private bool anyChanges = false;
    public ProjectManifest project = null;

    private string GetDatabasePath()
    {
        return project.faroRootDirectory + "\\bin\\guids_database.bin";
    }
}*/
