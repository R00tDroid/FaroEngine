#if FARO_OS_WEB
#include "WindowWeb.hpp"

namespace Faro
{
    void WindowWeb::Init()
    {
        Window::Init();
    }

    void WindowWeb::Destroy()
    {
        Window::Destroy();
    }

    void WindowWeb::SetTitle(String)
    {
    }

    void WindowWeb::SetWindowState(WindowState)
    {
    }

    WindowState WindowWeb::GetWindowState()
    {
        return {};
    }
}
#endif
