#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <filesystem>
#include <map>
#include <fstream>
#include <regex>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <uuid/uuid.h>
#endif

namespace Utility
{
    void Print(std::string log);

    void PrintLine(std::string log);

    void PrintD(std::string log);

    void PrintLineD(std::string log);

    void EnsureDirectory(std::filesystem::path path);

    std::string ToLower(std::string& Input);

    std::string ToUpper(std::string& Input);

    std::string GenerateUUID();

    std::string GetCachedUUID(std::filesystem::path storageLocation);

    std::filesystem::path GetExecutablePath();

#ifdef WIN32
    inline void HideFolder(std::filesystem::path folder)
    {
        SetFileAttributesA(folder.string().c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
#else
    inline void HideFolder(std::filesystem::path) {}
#endif

    bool MatchPattern(std::string source, std::string pattern, std::vector<std::string>& outMatches);

    bool ReadEnvVariable(std::string variableName, std::string& value);
}

class PerformanceTimer
{
public:
    static void StartGlobalTimer();

    static double GetMillisSinceStart();

    PerformanceTimer();

    ~PerformanceTimer();

    void Stop(std::string label);

    static void PrintTimers();

private:
    inline static std::chrono::high_resolution_clock::time_point AppStart;
    inline static int globalDepth = 0;
    inline static int timerCount = 0;
    int depth = 0;
    int index = 0;
    std::chrono::high_resolution_clock::time_point timer = {};

    inline static std::map<int, std::string> timerReports;
};
