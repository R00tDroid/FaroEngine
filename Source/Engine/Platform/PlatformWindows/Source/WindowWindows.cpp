#include "WindowWindows.hpp"

namespace Faro
{
    void WindowThread::ThreadInit()
    {
    }

    void WindowThread::ThreadUpdate()
    {
    }

    void WindowThread::ThreadDestroy()
    {
    }

    void WindowWindows::Init()
    {
        Window::Init();
        windowThread.Start();
    }

    void WindowWindows::Destroy()
    {
        windowThread.WaitForStop();
        Window::Destroy();
    }

    void WindowWindows::SetTitle(String title)
    {
    }

    void WindowWindows::SetSize(Int2D size)
    {
    }

    Int2D WindowWindows::GetSize()
    {
        return {};
    }
}
