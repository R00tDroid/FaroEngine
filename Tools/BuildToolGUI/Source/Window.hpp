#pragma once
#include "sciter-x-window.hpp"

class AppWindow : public sciter::window
{
public:
    AppWindow(RECT frame = RECT()) : window(SW_ALPHA | SW_POPUP | SW_MAIN | SW_ENABLE_DEBUG, frame) {}

    SOM_PASSPORT_BEGIN(AppWindow)
    SOM_FUNCS(
        SOM_FUNC(InvokeCommand)
    )
    SOM_PASSPORT_END

    void InvokeCommand(sciter::string command, sciter::string project);
};
