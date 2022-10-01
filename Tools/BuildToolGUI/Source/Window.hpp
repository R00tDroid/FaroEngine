#pragma once
#include "sciter-x-window.hpp"

class AppWindow : public sciter::window
{
public:
    AppWindow() : window(SW_ALPHA | SW_POPUP | SW_MAIN | SW_ENABLE_DEBUG) {}

    SOM_PASSPORT_BEGIN(AppWindow)
    SOM_FUNCS(
        SOM_FUNC(InvokeCommand)
    )
    SOM_PASSPORT_END

    void InvokeCommand(sciter::string command);
};
