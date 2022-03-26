using System;
using System.Collections.Generic;

public class ParameterList
{
    public ParameterList(String[] input)
    {
        PerformanceTimer timer = new PerformanceTimer();
        Utility.PrintLineD("Parsing ParameterList...");

        String lastAdded = "";
        
        foreach (String arg in input)
        {
            String argl = arg.ToLower();
            if (argl[0] == '-')
            {
                argl = argl.Substring(1);
                parameters.Add(argl, new List<String>());
                lastAdded = argl;
            }
            else if(lastAdded.Length > 0)
            {
                parameters[lastAdded].Add(arg);
            }
            else
            {
                Utility.PrintLineD("Invalid parameter input: " + argl);
            }
        }
        
        Utility.PrintLineD("Parsed " + input.Length + " input(s) to " + parameters.Count + " parameter(s)");
        timer.Stop("Parse commandline");
    }

    public bool Contains(String check)
    {
        return parameters.ContainsKey(check);
    }

    public bool HasArguments(String check)
    {
        return Contains(check) && parameters[check].Count > 0;
    }

    public String[] GetArguments(String check)
    {
        return parameters[check].ToArray();
    }

    private Dictionary<String, List<String>> parameters = new Dictionary<string, List<string>>();
}