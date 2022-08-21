#include <iostream>
#include <Windows.h>
#include <string>

namespace Utility
{
    inline void Print(std::string log)
    {
        std::cout << log.c_str();
        OutputDebugStringA(log.c_str());
    }

    inline void PrintLine(std::string log)
    {
        Print(log + "\n");
    }

    inline void PrintD(std::string log)
    {
#ifndef NDEBUG
        Print("[D] " + log);
#endif
    }

    inline void PrintLineD(std::string log)
    {
#ifndef NDEBUG
        PrintD(log + "\n");
#endif
    }
}

class PerformanceTimer
{
public:
    static void StartGlobalTimer()
    {
        //applicationTimer = Stopwatch.StartNew();
    }

    static float GetMillisSinceStart()
    {
        return 0.0f;
    }

    PerformanceTimer()
    {
    }

    ~PerformanceTimer()
    {
    }

    public void Stop(std::string label)
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
        Utility::PrintLine("\n--Perf report--");
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
