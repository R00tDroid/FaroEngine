#include "Window.hpp"
#include "Console.hpp"

void AppWindow::InvokeCommand(sciter::string command, sciter::string project)
{
    Console::ExecuteCommand(command, project);
}
