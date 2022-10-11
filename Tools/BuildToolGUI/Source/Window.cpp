#include "Window.hpp"
#include "Console.hpp"

void AppWindow::InvokeCommand(sciter::string command, sciter::string project)
{
    Console::ExecuteCommand(command, project);
}

void AppWindow::MoveWindow(sciter::string dx, sciter::string dy)
{
    RECT rect;
    GetWindowRect(get_hwnd(), &rect);

    SetWindowPosition(rect.left + _wtoi(dx.c_str()), rect.top + _wtoi(dy.c_str()));
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
