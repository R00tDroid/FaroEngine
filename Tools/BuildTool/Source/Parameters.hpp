using System;
using System.Collections.Generic;

public class ParameterList
{
    public ParameterList(std::string[] input)
    {
        PerformanceTimer timer = new PerformanceTimer();
        Utility::PrintLineD("Parsing ParameterList...");

        std::string lastAdded = "";
        
        foreach (std::string arg in input)
        {
            std::string argl = arg.ToLower();
            if (argl[0] == '-')
            {
                argl = argl.Substring(1);
                parameters.Add(argl, new List<std::string>());
                lastAdded = argl;
            }
            else if(lastAdded.Length > 0)
            {
                parameters[lastAdded].Add(arg);
            }
            else
            {
                Utility::PrintLineD("Invalid parameter input: " + argl);
            }
        }
        
        Utility::PrintLineD("Parsed " + input.Length + " input(s) to " + parameters.Count + " parameter(s)");
        timer.Stop("Parse commandline");
    }

    public bool Contains(std::string check)
    {
        return parameters.ContainsKey(check);
    }

    public bool HasArguments(std::string check)
    {
        return Contains(check) && parameters[check].Count > 0;
    }

    public int CountArguments(std::string check)
    {
        return Contains(check) ? parameters[check].Count : 0;
    }

    public std::string[] GetArguments(std::string check)
    {
        return parameters[check].ToArray();
    }

    private Dictionary<std::string, List<std::string>> parameters = new Dictionary<std::string, List<std::string>>();
}