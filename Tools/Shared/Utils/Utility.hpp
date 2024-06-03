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
    inline void Print(std::string log)
    {
        std::cout << log.c_str() << std::flush;
        #ifdef WIN32
        OutputDebugStringA(log.c_str());
        #endif
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
    inline void EnsureDirectory(std::filesystem::path path)
    {
        if (!std::filesystem::exists(path)) 
        {
            std::filesystem::create_directories(path);
        }
    }

    inline std::string ToLower(std::string& Input)
    {
        std::string Result = Input;
        std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });
        return Result;
    }

    inline std::string ToUpper(std::string& Input)
    {
        std::string Result = Input;
        std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });
        return Result;
    }

    inline std::string GenerateUUID()
    {
#ifdef WIN32
        UUID uuid;
        if (UuidCreate(&uuid) != RPC_S_OK) { Utility::PrintLine("Failed to generate uuid"); return {}; }

        unsigned char* uuidString = nullptr;
        if (UuidToStringA(&uuid, &uuidString) != RPC_S_OK) { Utility::PrintLine("Failed to process uuid"); return {}; }

        std::string string(reinterpret_cast<char*>(uuidString));

        RpcStringFreeA(&uuidString);
#else
        uuid_t uuid;
        uuid_generate_random(uuid);
        char uuidString[37];
        uuid_unparse(uuid, uuidString);
        std::string string(uuidString);
#endif
        return ToUpper(string);
    }

    inline std::string GetCachedUUID(std::filesystem::path storageLocation)
    {
        std::ifstream uuidFile(storageLocation);
        if (uuidFile.is_open())
        {
            std::string uuid;
            uuidFile >> uuid;
            uuidFile.close();
            return ToUpper(uuid);
        }
        else
        {
            std::string uuid = Utility::GenerateUUID();
            std::ofstream uuidOutFile(storageLocation);
            uuidOutFile << uuid;
            uuidOutFile.close();
            return uuid;
        }
    }

    inline std::filesystem::path GetExecutablePath()
    {
#ifdef WIN32
        char filename[MAX_PATH];
        GetModuleFileNameA(nullptr, filename, MAX_PATH);
        return filename;
#else
        return std::filesystem::canonical("/proc/self/exe");
#endif
    }

#ifdef WIN32
    inline void HideFolder(std::filesystem::path folder)
    {
        SetFileAttributesA(folder.string().c_str(), FILE_ATTRIBUTE_HIDDEN);
    }
#else
    inline void HideFolder(std::filesystem::path) {}
#endif

    inline bool MatchPattern(std::string source, std::string pattern, std::vector<std::string>& outMatches)
    {
        std::smatch matches;
        if (!std::regex_search(source, matches, std::regex(pattern)))
        {
            return false;
        }

        for (size_t i = 1; i < matches.size(); i++)
        {
            outMatches.push_back(matches[i]);
        }

        return true;
    }

    bool ReadEnvVariable(std::string variableName, std::string& value);
}

class PerformanceTimer
{
public:
    static void StartGlobalTimer()
    {
        AppStart = std::chrono::high_resolution_clock::now();
    }

    static double GetMillisSinceStart()
    {
        std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
        const long long microseconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(Now - AppStart).count());
        return static_cast<double>(microseconds / 1000.0);
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
        const long long microseconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(Now - timer).count());

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
