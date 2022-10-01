#include "Console.hpp"
#include "Window.hpp"

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
        Append(L"Could not find FaroBuildTool at " + buildTool.wstring());
        return;
    }

    Append(L"Command: " + command + L"\n");
}

void Console::Clear()
{
    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleClear");
    AppWindow::broadcast_event(params);
}

void Console::Append(std::wstring string)
{
    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("consoleAppend");
    params.data = string;
    AppWindow::broadcast_event(params);
}
