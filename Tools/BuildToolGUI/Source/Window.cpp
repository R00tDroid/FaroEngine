#include "Window.hpp"
#include "Console.hpp"

void AppWindow::InvokeCommand(sciter::string command, sciter::string platform, sciter::string config, sciter::string project)
{
    Console::ExecuteCommand(command, platform, config, project);
}

void AppWindow::MoveWindow(sciter::string dx, sciter::string dy)
{
    RECT rect;
    GetWindowRect(get_hwnd(), &rect);

    SetWindowPosition(rect.left + _wtoi(dx.c_str()), rect.top + _wtoi(dy.c_str()));
}

void AppWindow::SetWindowState(sciter::string state)
{
    if (state == sciter::string(L"minimize"))
    {
        collapse();
    }
    else if (state == sciter::string(L"close"))
    {
        request_close();
    }  
}

void AppWindow::OpenProjectFile(sciter::string currentFile)
{

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

void AppWindow::SetProjectPath(std::wstring path)
{
    BEHAVIOR_EVENT_PARAMS params = { 0 };
    params.name = WSTR("setProjectPath");
    params.data = path;
    AppWindow::broadcast_event(params);
}
