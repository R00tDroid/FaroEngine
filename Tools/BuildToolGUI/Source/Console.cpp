#include "Console.hpp"
#include "Window.hpp"

void Console::ExecuteCommand(std::wstring command)
{
    Clear();
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
