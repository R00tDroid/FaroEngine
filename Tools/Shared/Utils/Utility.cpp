#include "Utility.hpp"

#include <iostream>
#include <fstream>
#include <regex>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <uuid/uuid.h>
#endif

void Utility::Print(std::string log)
{
    std::cout << log.c_str() << std::flush;
#ifdef WIN32
    OutputDebugStringA(log.c_str());
#endif
}

void Utility::PrintLine(std::string log)
{
    Print(log + "\n");
}

void Utility::PrintD(std::string log)
{
#ifndef NDEBUG
    Print("[D] " + log);
#endif
}

void Utility::PrintLineD(std::string log)
{
#ifndef NDEBUG
    PrintD(log + "\n");
#endif
}

void Utility::EnsureDirectory(std::filesystem::path path)
{
    if (!std::filesystem::exists(path)) 
    {
        std::filesystem::create_directories(path);
    }
}

std::string Utility::ToLower(std::string& Input)
{
    std::string Result = Input;
    std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });
    return Result;
}

std::string Utility::ToUpper(std::string& Input)
{
    std::string Result = Input;
    std::transform(Result.begin(), Result.end(), Result.begin(), [](char c) { return static_cast<char>(std::toupper(c)); });
    return Result;
}

std::string Utility::GenerateUUID()
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

std::string Utility::GetCachedUUID(std::filesystem::path storageLocation)
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

std::filesystem::path Utility::GetExecutablePath()
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
void Utility::HideFolder(std::filesystem::path folder)
{
    SetFileAttributesA(folder.string().c_str(), FILE_ATTRIBUTE_HIDDEN);
}
#else
void Utility::HideFolder(std::filesystem::path) {}
#endif

bool Utility::MatchPattern(std::string source, std::string pattern, std::vector<std::string>& outMatches)
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

bool Utility::ReadEnvVariable(std::string variableName, std::string& value)
{
#ifdef WIN32
    size_t size = 0;
    if (getenv_s(&size, nullptr, 0, variableName.c_str()) != 0)
    {
        return false;
    }
    value.resize(size);
    if (getenv_s(&size, value.data(), size, variableName.c_str()) != 0)
    {
        return false;
    }
#else
    value = std::string(getenv(variableName.c_str()));
#endif

    // Remove null-terminator at end of string
    if (value.back() == '\0')
    {
        value.pop_back();
    }

    return true;
}

void PerformanceTimer::StartGlobalTimer()
{
    AppStart = std::chrono::high_resolution_clock::now();
}

double PerformanceTimer::GetMillisSinceStart()
{
    std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
    const long long microseconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(Now - AppStart).count());
    return static_cast<double>(microseconds / 1000.0);
}

PerformanceTimer::PerformanceTimer()
{
    timer = std::chrono::high_resolution_clock::now();

    depth = globalDepth;
    globalDepth++;

    index = timerCount;
    timerCount++;
}

PerformanceTimer::~PerformanceTimer()
{
}

void PerformanceTimer::Stop(std::string label)
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

void PerformanceTimer::PrintTimers()
{
    Utility::PrintLine("\n--Perf report--");
    for (const auto& report : timerReports)
    {
        Utility::PrintLine(report.second);
    }
}
