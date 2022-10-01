#include "Window.hpp"
#include "Console.hpp"

void AppWindow::InvokeCommand(sciter::string command)
{
    Console::ExecuteCommand(command);
}
