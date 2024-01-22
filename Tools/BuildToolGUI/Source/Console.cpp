#include "Console.hpp"
#include "Window.hpp"
#include <array>
#include <mutex>
#include "Command.hpp"

std::filesystem::path Console::buildTool;

void Console::Init()
{
    //wchar_t binaryPath[MAX_PATH];
    //GetModuleFileNameW(nullptr, binaryPath, MAX_PATH);

    //std::filesystem::path exePath = binaryPath;
    //buildTool = exePath.parent_path() / "FaroBuildTool.exe";
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

    InvokeCommand(L"\"" + buildTool.wstring() + L"\" " + command + L" -project \"" + project + L"\" -platform " + platform + L" -" + config);
}

std::mutex invokeLock;

void Console::InvokeCommand(std::wstring command)
{
    Utility::ExecuteCommandAsync(Utility::Convert(command),
    []()
    {
        invokeLock.lock();
    },
    [](std::string logString)
    {
        Append(Utility::Convert(logString));
    },
    [](bool commandSuccess, int commandResult)
    {
        if (!commandSuccess)
        {
            AppendLine(L"Failed to invoke command");
        }
        else
        {
            if (commandResult == 0)
            {
                AppendLine(L"Task completed successfully");
            }
            else
            {
                AppendLine(L"Task failed with status code: " + std::to_wstring((int)commandResult));
            }
        }

        LockConsole(false);
        invokeLock.unlock();
    });
}

std::mutex consoleLock;

void Console::Clear()
{
    consoleLock.lock();

    /*BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleClear");
    AppWindow::broadcast_event(params);*/

    consoleLock.unlock();
}

void Console::Append(std::wstring string)
{
    consoleLock.lock();

    /*BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleAppend");
    params.data = string;
    AppWindow::broadcast_event(params);*/

    consoleLock.unlock();
}

void Console::AppendLine(std::wstring string)
{
    Append(string + L"\n");
}

void Console::LockConsole(bool)
{
    /*BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("lockConsole");
    params.data = lock ? L"true" : L"false";
    AppWindow::broadcast_event(params);*/
}
