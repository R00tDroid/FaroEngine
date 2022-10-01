#include "Resources.generated.hpp"
#include "sciter-x.h"
#include "Window.hpp"

int uimain(std::function<int()> run)
{
    sciter::archive::instance().open(aux::elements_of(resources));

    sciter::om::hasset<AppWindow> window = new AppWindow();

    window->load(WSTR("this://app/index.html"));

    window->expand();

    return run();
}
