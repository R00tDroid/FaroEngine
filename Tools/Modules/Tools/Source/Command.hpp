#pragma once
#include <functional>
#include <string>
#include "FaroToolsExports.generated.h"

namespace Utility
{
    //TODO Don't use STL
    extern int FaroToolsExports ExecuteCommand(std::string command, std::string& consoleOutput);

    //TODO Don't use STL
    extern void FaroToolsExports ExecuteCommandAsync(std::string command, std::function<void()> onBegin, std::function<void(std::string logString)> onLog, std::function<void(bool commandSuccess, int commandResult)> onExit);

    extern std::string Convert(std::wstring string);

    extern std::wstring Convert(std::string string);
}
