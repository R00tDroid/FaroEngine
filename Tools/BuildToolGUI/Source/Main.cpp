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
    
    sciter::string InvokeCommand(sciter::string command)
    {
        return sciter::string(L"okay: " + command);
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
