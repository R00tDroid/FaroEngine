#include "Console.hpp"
#include "Window.hpp"
#include <array>
#include <mutex>
#include <thread>

std::filesystem::path Console::buildTool;

void Console::Init()
{
    wchar_t binaryPath[MAX_PATH];
    GetModuleFileNameW(nullptr, binaryPath, MAX_PATH);

    std::filesystem::path exePath = binaryPath;
    buildTool = exePath.parent_path() / "FaroBuildTool.exe";
}

void Console::ExecuteCommand(std::wstring command, std::wstring platform, std::wstring config, std::wstring project)
{
    LockConsole(true);
    Clear();

    if (!std::filesystem::exists(buildTool))
    {
        AppendLine(L"Could not find FaroBuildTool at " + buildTool.wstring());
        return;
    }

    InvokeCommand(L"\"" + buildTool.wstring() + L"\" " + command + L" -project " + project + L" -platform " + platform + L" -" + config);
}

std::string Convert(std::wstring string)
{
    int stringLength = WideCharToMultiByte(CP_ACP, 0, string.c_str(), (int)string.length(), nullptr, 0, nullptr, nullptr);
    std::string str(stringLength, 0);
    WideCharToMultiByte(CP_ACP, 0, string.c_str(), -1, &str[0], stringLength, nullptr, nullptr);
    return str;
}

std::wstring Convert(std::string string)
{
    int stringLength = MultiByteToWideChar(CP_ACP, 0, string.c_str(), (int)string.length(), nullptr, 0);
    std::wstring wstr(stringLength, 0);
    MultiByteToWideChar(CP_ACP, 0, string.c_str(), (int)string.length(), &wstr[0], stringLength);
    return wstr;
}

std::mutex invokeLock;

void Console::InvokeCommand(std::wstring command)
{
    std::thread thread([command]()
    {
        invokeLock.lock();

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

        if (!CreateProcessW(nullptr, const_cast<wchar*>((command + L" 2>&1").c_str()), nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo))
        {
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);

            AppendLine(L"Failed to invoke command");
            LockConsole(false);
            invokeLock.unlock();
            return;
        }

        CloseHandle(stdOutRead);
        
        while (true)
        {
            DWORD readSize;
            CHAR logBuffer[128];
            if (!ReadFile(stdOutWrite, logBuffer, 128, &readSize, nullptr)) break;
            if (readSize == 0) break;
            std::string logString(logBuffer, readSize);
            Append(Convert(logString));
        }

        WaitForSingleObject(processInfo.hProcess, INFINITE);
        DWORD exitCode;
        GetExitCodeProcess(processInfo.hProcess, &exitCode);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        if (exitCode == 0)
        {
            AppendLine(L"Task completed successfully");
        }
        else
        {
            AppendLine(L"Task failed with status code: " + std::to_wstring((int)exitCode));
        }

        LockConsole(false);
        invokeLock.unlock();
    });

    thread.detach();
}

std::mutex consoleLock;

void Console::Clear()
{
    consoleLock.lock();

    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleClear");
    AppWindow::broadcast_event(params);

    consoleLock.unlock();
}

void Console::Append(std::wstring string)
{
    consoleLock.lock();

    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleAppend");
    params.data = string;
    AppWindow::broadcast_event(params);

    consoleLock.unlock();
}

void Console::AppendLine(std::wstring string)
{
    Append(string + L"\n");
}

void Console::LockConsole(bool lock)
{
    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("lockConsole");
    params.data = lock ? L"true" : L"false";
    AppWindow::broadcast_event(params);
}
