#pragma once

#include <string>
#include <chrono>
#include <filesystem>
#include <vector>
#include <FaroToolsExports.generated.h>

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

    void HideFolder(std::filesystem::path folder);

    bool MatchPattern(std::string source, std::string pattern, std::vector<std::string>& outMatches);

    bool MatchWildcard(const std::string& source, const std::string& pattern);

    bool ReadEnvVariable(std::string variableName, std::string& value);
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
