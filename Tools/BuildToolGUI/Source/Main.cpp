#include "sciter-x.h"
#include "sciter-x-window.hpp"

class frame: public sciter::window
{
public:
    frame() : window(SW_ALPHA | SW_POPUP | SW_MAIN | SW_ENABLE_DEBUG) {}

    SOM_PASSPORT_BEGIN(frame)
    SOM_FUNCS(
        SOM_FUNC(InvokeCommand)
    )
    SOM_PASSPORT_END
    
    void InvokeCommand(sciter::string command)
    {
        ConsoleClear();
        ConsoleAppend(sciter::string(L"Command: ") + command + sciter::string(L"\n"));
    }

    static void ConsoleClear()
    {
        BEHAVIOR_EVENT_PARAMS params = { 0 };
        params.name = WSTR("consoleClear");
        frame::broadcast_event(params);
    }

    static void ConsoleAppend(const sciter::string& string)
    {
        BEHAVIOR_EVENT_PARAMS params = { 0 };
        params.name = WSTR("consoleAppend");
        params.data = string;
        frame::broadcast_event(params);
    }
};

#include "Resources.generated.hpp"

int uimain(std::function<int()> run)
{
    sciter::archive::instance().open(aux::elements_of(resources));

    sciter::om::hasset<frame> pwin = new frame();

    pwin->load(WSTR("this://app/index.html"));

    pwin->expand();

    return run();
}
