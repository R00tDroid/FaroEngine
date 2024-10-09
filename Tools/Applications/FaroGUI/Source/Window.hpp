#pragma once
#include "sciter-x-window.hpp"

class AppWindow : public sciter::window
{
public:
    AppWindow(RECT frame = RECT()) : window(SW_ALPHA | SW_POPUP | SW_MAIN | SW_ENABLE_DEBUG, frame) {}

    SOM_PASSPORT_BEGIN(AppWindow)
    SOM_FUNCS(
        SOM_FUNC(InvokeCommand),
        SOM_FUNC(MoveWindow),
        SOM_FUNC(SetWindowState),
        SOM_FUNC(OpenProjectFile)
    )
    SOM_PASSPORT_END

    void InvokeCommand(sciter::string command, sciter::string platform, sciter::string config, sciter::string project);

    void MoveWindow(sciter::string dx, sciter::string dy);

    void SetWindowState(sciter::string state);

    void OpenProjectFile(sciter::string currentFile);

    void GetWindowSize(int& x, int& y);

    void SetWindowPosition(int x, int y);

    void SetProjectPath(std::wstring path);
};
