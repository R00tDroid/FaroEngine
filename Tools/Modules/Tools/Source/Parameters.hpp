#pragma once
#include <algorithm>
#include <map>
#include <string>
#include "Utility.hpp"

class ParameterList
{
public:
    ParameterList(int argc, const char** argv)
    {
        if (argc == 0 || argv == nullptr) return;

        PerformanceTimer timer;
        Utility::PrintLineD("Parsing ParameterList...");

        std::string lastAdded = "";

        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            std::string argl = Utility::ToLower(arg);

            if (argl[0] == '-')
            {
                argl = argl.substr(1);
                parameters.insert(std::pair<std::string, std::vector<std::string>>(argl, {}));
                lastAdded = argl;
            }
            else if (!lastAdded.empty())
            {
                parameters[lastAdded].push_back(arg);
            }
            else
            {
                Utility::PrintLineD("Invalid parameter input: " + argl);
            }
        }

        Utility::PrintLineD("Parsed " + std::to_string(argc - 1) + " input(s) to " + std::to_string(parameters.size()) + " parameter(s)");
        timer.Stop("Parse commandline");
    }

    bool Contains(std::string check)
    {
        return parameters.find(check) != parameters.end();
    }

    bool HasArguments(std::string check)
    {
        return Contains(check) && !parameters[check].empty();
    }

    int CountArguments(std::string check)
    {
        return Contains(check) ? int(parameters[check].size()) : 0;
    }

    std::vector<std::string> GetArguments(std::string check)
    {
        return parameters[check];
    }

private:
    std::map<std::string, std::vector<std::string>> parameters;
};
