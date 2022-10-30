#include "Command.hpp"
#include <array>

#ifdef WIN32
#define popen_impl _popen
#define pclose_impl _pclose
#else
#define popen_impl popen
#define pclose_impl pclose
#endif

int ExecuteCommand(std::string command, std::string& consoleOutput)
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
