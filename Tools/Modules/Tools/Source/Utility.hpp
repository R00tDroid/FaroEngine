#pragma once

#include <string>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <vector>
#include <FaroToolsExports.generated.h>

#define UUID_LENGTH 36

namespace Utility
{
    void FaroToolsExports Print(const char* log);
    inline void Print(const std::string& log) { Print(log.c_str()); }
    inline void Print(const std::string&& log) { Print(log); }

    void FaroToolsExports PrintLine(const char* log);
    inline void PrintLine(const std::string& log) { PrintLine(log.c_str()); }
    inline void PrintLine(const std::string&& log) { PrintLine(log); }

    void FaroToolsExports PrintD(const char* log);
    inline void PrintD(const std::string& log) { PrintD(log.c_str()); }
    inline void PrintD(const std::string&& log) { PrintD(log); }

    void FaroToolsExports PrintLineD(const char* log);
    inline void PrintLineD(const std::string& log) { PrintLineD(log.c_str()); }
    inline void PrintLineD(const std::string&& log) { PrintLineD(log); }

    void FaroToolsExports EnsureDirectory(const char* path);

    inline std::string ToLower(const std::string& Input)
    {
        std::string Result = Input;
        std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });
        return Result;
    }

    inline std::string ToUpper(const std::string& Input)
    {
        std::string Result = Input;
        std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });
        return Result;
    }

    bool FaroToolsExports GenerateUUID(char* uuid);

    bool FaroToolsExports GetCachedUUID(const char* storageLocation, char* uuid);

    //TODO Don't use stl containers
    std::filesystem::path FaroToolsExports GetExecutablePath();

    void FaroToolsExports HideFolder(const char* folder);

    //TODO Don't use stl containers
    bool FaroToolsExports MatchPattern(std::string source, std::string pattern, std::vector<std::string>& outMatches);

    bool FaroToolsExports MatchWildcard(const char* source, const char* pattern);

    //TODO Don't use stl containers
    bool FaroToolsExports ReadEnvVariable(std::string variableName, std::string& value);

    bool FaroToolsExports IsSourceFile(const char* file);
    bool FaroToolsExports IsIncludeFile(const char* file);
}

class FaroToolsExports PerformanceTimer
{
public:
    static void StartGlobalTimer();

    static double GetMillisSinceStart();

    PerformanceTimer();
    ~PerformanceTimer();

    void Stop(const char* label);

    static void PrintTimers();

private:
    struct Impl;
    Impl* impl = nullptr;
};
