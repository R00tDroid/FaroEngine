#if FARO_OS_ANDROID
#include "WindowAndroid.hpp"

namespace Faro
{
    void WindowAndroid::Init()
    {
        Window::Init();
    }

    void WindowAndroid::Destroy()
    {
        Window::Destroy();
    }

    void WindowAndroid::SetTitle(String)
    {
    }

    void WindowAndroid::SetWindowState(WindowState)
    {
    }

    WindowState WindowAndroid::GetWindowState()
    {
        return {};
    }
}
#endif
