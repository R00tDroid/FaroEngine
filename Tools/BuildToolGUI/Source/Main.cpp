#include "sciter-x.h"
#include "sciter-x-window.hpp"

class frame: public sciter::window
{
public:
    frame() : window(SW_ALPHA | SW_POPUP | SW_MAIN | SW_ENABLE_DEBUG) {}

    // passport - lists native functions and properties exposed to script under 'frame' interface name:
    SOM_PASSPORT_BEGIN(frame)
    SOM_FUNCS(
        SOM_FUNC(nativeMessage)
    )
    SOM_PASSPORT_END

    // function expsed to script:
    sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

};

#include "Resources.generated.hpp"

int uimain(std::function<int()> run )
{
    sciter::archive::instance().open(aux::elements_of(resources));

    sciter::om::hasset<frame> pwin = new frame();

    pwin->load(WSTR("this://app/index.html"));

    pwin->expand();

    return run();
}