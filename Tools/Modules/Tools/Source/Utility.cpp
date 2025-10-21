#include "Utility.hpp"

#include <map>
#include <iostream>
#include <fstream>
#include <mutex>
#include <regex>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <uuid/uuid.h>
#endif

std::mutex logLock;

void writeLog(const char* log)
{
    std::cout << log << std::flush;
#ifdef WIN32
    OutputDebugStringA(log);
#endif
}

void Utility::Print(const char* log)
{
    logLock.lock();
    writeLog(log);
    logLock.unlock();
}

void Utility::PrintLine(const char* log)
{
    logLock.lock();
    writeLog(log);
    writeLog("\n");
    logLock.unlock();
}

#ifdef NDEBUG
void Utility::PrintD(const char*) {}
void Utility::PrintLineD(const char*) {}
#else
void Utility::PrintD(const char* log)
{
    logLock.lock();
    writeLog("[D] ");
    writeLog(log);
    logLock.unlock();
}
void Utility::PrintLineD(const char* log)
{
    logLock.lock();
    writeLog("[D] ");
    writeLog(log);
    writeLog("\n");
    logLock.unlock();

}
#endif

void Utility::EnsureDirectory(const char* path)
{
    if (!std::filesystem::exists(path)) 
    {
        std::filesystem::create_directories(path);
    }
}

std::string GenerateUUIDInternal()
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

    Utility::ToUpper(string);
    if (string.length() > UUID_LENGTH) string.resize(UUID_LENGTH);
    return string;
}

bool Utility::GenerateUUID(char* outUuid)
{
    std::string uuid = GenerateUUIDInternal();
    memset(outUuid, 0, UUID_LENGTH);
    memcpy(outUuid, uuid.c_str(), uuid.length());

    return true;
}

bool Utility::GetCachedUUID(const char* storageLocation, char* outUuid)
{
    std::string uuid;

    std::ifstream uuidFile(storageLocation);
    if (uuidFile.is_open())
    {
        uuidFile >> uuid;
        uuidFile.close();
    }
    else
    {
        uuid = GenerateUUIDInternal();
        std::ofstream uuidOutFile(storageLocation);
        uuidOutFile << uuid;
        uuidOutFile.close();
    }

    ToUpper(uuid);
    if (uuid.length() > UUID_LENGTH) uuid.resize(UUID_LENGTH);

    memset(outUuid, 0, UUID_LENGTH);
    memcpy(outUuid, uuid.c_str(), uuid.length());

    return true;
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
void Utility::HideFolder(const char* folder)
{
    SetFileAttributesA(folder, FILE_ATTRIBUTE_HIDDEN);
}
#else
void Utility::HideFolder(const char*) {}
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

bool MatchWildcardInternal(const std::string& source, const std::string& pattern, size_t sourceIndex, size_t patternIndex)
{
    if (pattern.length() == patternIndex)
    {
        return source.length() == sourceIndex;
    }
    else if (pattern[patternIndex] == '*') 
    {
        for (; source[sourceIndex] != '\0'; sourceIndex++)
        {
            if (MatchWildcardInternal(source, pattern, sourceIndex, patternIndex + 1))
            {
                return true;
            }
        }
        return MatchWildcardInternal(source, pattern, sourceIndex, patternIndex + 1);
    }
    else if (pattern[patternIndex] != '?' && pattern[patternIndex] != source[sourceIndex]) 
    {
        return false;
    }
    else 
    {
        return MatchWildcardInternal(source, pattern, sourceIndex + 1, patternIndex + 1);
    }
}

bool Utility::MatchWildcard(const char* source, const char* pattern)
{
    std::string sourceString(source);
    std::string patternString(pattern);

    return MatchWildcardInternal(sourceString, patternString, 0, 0);
}

bool Utility::ReadEnvVariable(std::string variableName, std::string& value)
{
#ifdef WIN32
    size_t size = 0;
    if (getenv_s(&size, nullptr, 0, variableName.c_str()) != 0)
    {
        return false;
    }
    if (size == 0)
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

static std::vector<std::string> sourceExtensions = { ".cpp", ".c", ".hlsl" };
static std::vector<std::string> includeExtensions = { ".hpp", ".h", ".hlsli" };

bool Utility::IsSourceFile(const char* file)
{
    std::string extension = std::filesystem::path(file).extension().string();
    extension = ToLower(extension);
    return std::find(sourceExtensions.begin(), sourceExtensions.end(), extension) != sourceExtensions.end();
}

bool Utility::IsIncludeFile(const char* file)
{
    std::string extension = std::filesystem::path(file).extension().string();
    extension = ToLower(extension);
    return std::find(includeExtensions.begin(), includeExtensions.end(), extension) != includeExtensions.end();
}

struct PerformanceTimerStatic
{
    std::chrono::high_resolution_clock::time_point appStart;
    int globalDepth = 0;
    int timerCount = 0;
    std::map<int, std::string> timerReports;
} performanceTimer_static;

struct PerformanceTimer::Impl
{
    int depth = 0;
    int index = 0;
    std::chrono::high_resolution_clock::time_point timer = {};
};

void PerformanceTimer::StartGlobalTimer()
{
    performanceTimer_static.appStart = std::chrono::high_resolution_clock::now();
}

double PerformanceTimer::GetMillisSinceStart()
{
    std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
    const long long microseconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(Now - performanceTimer_static.appStart).count());
    return static_cast<double>(microseconds / 1000.0);
}

PerformanceTimer::PerformanceTimer()
{
    impl = new Impl();
    impl->timer = std::chrono::high_resolution_clock::now();

    impl->depth = performanceTimer_static.globalDepth;
    performanceTimer_static.globalDepth++;

    impl->index = performanceTimer_static.timerCount;
    performanceTimer_static.timerCount++;
}

PerformanceTimer::~PerformanceTimer()
{
    delete impl;
}

void PerformanceTimer::Stop(const char* label)
{
    performanceTimer_static.globalDepth--;

    std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
    const long long microseconds = static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(Now - impl->timer).count());

    std::string message = "";
    for (int i = 0; i < impl->depth; i++)
    {
        message += " |";
    }
    message += "-*>";
    message += "[" + std::string(label) + "] " + std::to_string(microseconds / 1000.0f);

    performanceTimer_static.timerReports.insert(std::pair<int, std::string>(impl->index, message));
}

void PerformanceTimer::PrintTimers()
{
    Utility::PrintLine("\n--Perf report--");
    for (const auto& report : performanceTimer_static.timerReports)
    {
        Utility::PrintLine(report.second);
    }
}
