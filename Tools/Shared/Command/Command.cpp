#include "Command.hpp"
#include <array>
#include <thread>

#ifdef WIN32
#include <windows.h>
#define popen_impl _popen
#define pclose_impl _pclose
#else
#define popen_impl popen
#define pclose_impl pclose
#endif

int Utility::ExecuteCommand(std::string command, std::string& consoleOutput)
{
    std::array<char, 16> logBuffer{};
    FILE* processPipe = popen_impl((command + " 2>&1").c_str(), "r");
    if (!processPipe)
    {
        return -1;
    }
    while (fgets(logBuffer.data(), int(logBuffer.size()), processPipe) != nullptr)
    {
        consoleOutput += logBuffer.data();
    }

    return pclose_impl(processPipe);
}

void Utility::ExecuteCommandAsync(std::string command, std::function<void()> onBegin, std::function<void(std::string logString)> onLog, std::function<void(bool commandSuccess, int commandResult)> onExit)
{
#ifdef WIN32
    std::thread thread([command, onBegin, onLog, onExit]()
    {
        onBegin();

        HANDLE stdOutWrite = nullptr;
        HANDLE stdOutRead = nullptr;

        SECURITY_ATTRIBUTES securityDesc;
        securityDesc.nLength = sizeof(SECURITY_ATTRIBUTES);
        securityDesc.bInheritHandle = true;
        securityDesc.lpSecurityDescriptor = nullptr;
        CreatePipe(&stdOutWrite, &stdOutRead, &securityDesc, 0);
        SetHandleInformation(stdOutWrite, HANDLE_FLAG_INHERIT, 0);

        PROCESS_INFORMATION processInfo;
        ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

        STARTUPINFOW startupInfo;
        ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
        startupInfo.cb = sizeof(STARTUPINFO);
        startupInfo.hStdOutput = stdOutRead;
        startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        if (!CreateProcessW(nullptr, const_cast<wchar_t*>((Convert(command) + L" 2>&1").c_str()), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo))
        {
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);

            onExit(false, 0);
        }

        CloseHandle(stdOutRead);

        while (true)
        {
            DWORD readSize;
            CHAR logBuffer[128];
            if (!ReadFile(stdOutWrite, logBuffer, 128, &readSize, nullptr)) break;
            if (readSize == 0) break;
            std::string logString(logBuffer, readSize);
            onLog(logString);
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        onExit(true, (int)exitCode);
    });

    thread.detach();
#else
    onBegin();
    onLog("Async execution not supported for: " + command);
    onExit(false, -1);
#endif
}

std::string Utility::Convert(std::wstring string)
{
#ifdef WIN32
    size_t stringLength;
    if (0 != wcstombs_s(&stringLength, nullptr, 0, string.c_str(), string.length()))
    {
        return {};
    }
    std::string str(stringLength, 0);
    if (0 != wcstombs_s(&stringLength, str.data(), stringLength, string.c_str(), string.length()))
    {
        return {};
    }
#else
    size_t stringLength = wcstombs(nullptr, string.c_str(), string.length());
    std::string str(stringLength, 0);
    wcstombs(str.data(), string.c_str(), string.length());
#endif
    return str;
}

std::wstring Utility::Convert(std::string string)
{
#ifdef WIN32
    size_t stringLength;
    if (0 != mbstowcs_s(&stringLength, nullptr, 0, string.c_str(), string.length()))
    {
        return {};
    }
    std::wstring wstr(stringLength, 0);
    if (0 != mbstowcs_s(&stringLength, wstr.data(), stringLength, string.c_str(), string.length()))
    {
        return {};
    }
#else
    size_t stringLength = mbstowcs(nullptr, string.c_str(), string.length());
    std::wstring wstr(stringLength, 0);
    mbstowcs(wstr.data(), string.c_str(), string.length());
#endif
    return wstr;
}
