#pragma once

#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <map>

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
        AppStart = std::chrono::high_resolution_clock::now();
    }

    static float GetMillisSinceStart()
    {
        std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(Now - AppStart).count() / 1000.0f;
    }

    PerformanceTimer()
    {
        timer = std::chrono::high_resolution_clock::now();

        depth = globalDepth;
        globalDepth++;

        index = timerCount;
        timerCount++;
    }

    ~PerformanceTimer()
    {
    }

    void Stop(std::string label)
    {
        globalDepth--;

        std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
        long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(Now - timer).count();

        std::string message = "";
        for (int i = 0; i < depth; i++)
        {
            message += " |";
        }
        message += "-*>";
        message += "[" + label + "] " + std::to_string(microseconds / 1000.0f);

        timerReports.insert(std::pair<int, std::string>(index, message));
    }

    static void PrintTimers()
    {
        Utility::PrintLine("\n--Perf report--");
        for (const auto& report : timerReports)
        {
            Utility::PrintLine(report.second);
        }
    }

private:
    inline static std::chrono::high_resolution_clock::time_point AppStart;
    inline static int globalDepth = 0;
    inline static int timerCount = 0;
    int depth = 0;
    int index = 0;
    std::chrono::high_resolution_clock::time_point timer = {};

    inline static std::map<int, std::string> timerReports;
};
