#include "WindowWindows.hpp"
#include <MinWindows.hpp>

#define WindowClass "FaroWindow"

namespace Faro
{
    LRESULT MessageProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowThread* parentThread = reinterpret_cast<WindowThread*>(GetWindowLongPtrA(handle, GWLP_USERDATA));

        return DefWindowProcA(handle, message, wParam, lParam);
    }

    void WindowThread::ThreadInit()
    {
        HINSTANCE processHandle = GetModuleHandleA(nullptr);

        WNDCLASSEXA windowThread;
        ZeroMemory(&windowThread, sizeof(windowThread));

        windowThread.hInstance = processHandle;
        windowThread.lpszClassName = WindowClass;
        windowThread.lpfnWndProc = MessageProc;
        windowThread.style = CS_DBLCLKS;
        windowThread.cbSize = sizeof(WNDCLASSEXA);
        windowThread.lpszMenuName = nullptr;
        windowThread.cbClsExtra = 0;
        windowThread.cbWndExtra = 0;
        windowThread.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

        windowHandle = CreateWindowExA(0, WindowClass, nullptr, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, nullptr, nullptr, processHandle, nullptr);

        SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    void WindowThread::ThreadUpdate()
    {
        if (windowHandle != 0) 
        {
            MSG message;
            while (PeekMessageA(&message, windowHandle, 0, 0, true))
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }
        }

        //TODO reduce cpu load
    }

    void WindowThread::ThreadDestroy()
    {
        if (windowHandle != 0) 
        {
            DestroyWindow(windowHandle);
            windowHandle = 0;
        }
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
