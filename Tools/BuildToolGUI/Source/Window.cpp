#include "Window.hpp"
#include "Console.hpp"

void AppWindow::InvokeCommand(sciter::string command, sciter::string project)
{
    Console::ExecuteCommand(command, project);
}

void AppWindow::GetWindowSize(int& x, int& y)
{
    RECT rect;
    GetWindowRect(get_hwnd(), &rect);
    x = rect.right - rect.left;
    y = rect.bottom - rect.top;
}

void AppWindow::SetWindowPosition(int x, int y)
{
    SetWindowPos(get_hwnd(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
}
