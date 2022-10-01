#include "Console.hpp"
#include "Window.hpp"
#include <array>
#include <codecvt>
#include <mutex>
#include <thread>

std::filesystem::path Console::buildTool;

void Console::Init()
{
    wchar_t szPath[MAX_PATH];
    GetModuleFileNameW(nullptr, szPath, MAX_PATH);

    std::filesystem::path exePath = szPath;
    buildTool = exePath.parent_path() / "FaroBuildTool.exe";
}

void Console::ExecuteCommand(std::wstring command)
{
    Clear();

    if (!std::filesystem::exists(buildTool))
    {
        AppendLine(L"Could not find FaroBuildTool at " + buildTool.wstring());
        return;
    }

    InvokeCommand(L"\"" + buildTool.wstring() + L"\" " + command);
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

        std::array<char, 16> logBuffer{};

        FILE* processPipe = _popen((Convert(command) + " 2>&1").c_str(), "r");
        if (!processPipe)
        {
            AppendLine(L"Failed to invoke command");
            invokeLock.unlock();
            return;
        }
        while (fgets(logBuffer.data(), int(logBuffer.size()), processPipe) != nullptr)
        {
            std::string log(logBuffer.data());
            Append(Convert(log));
        }

        int result = _pclose(processPipe);
        AppendLine(L"Result: " + std::to_wstring(result));

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
