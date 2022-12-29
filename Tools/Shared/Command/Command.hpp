#pragma once
#include <functional>
#include <string>

namespace Utility
{
    extern int ExecuteCommand(std::string command, std::string& consoleOutput);

    extern void ExecuteCommandAsync(std::string command, std::function<void()> onBegin, std::function<void(std::string logString)> onLog, std::function<void(bool commandSuccess, int commandResult)> onExit);

    extern std::string Convert(std::wstring string);

    extern std::wstring Convert(std::string string);
}
