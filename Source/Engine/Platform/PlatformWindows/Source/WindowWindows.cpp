#include "WindowWindows.hpp"
#include <MinWindows.hpp>
#include "PlatformWindowsLog.hpp"

#define WindowClass "FaroWindow"

namespace Faro
{
    Mutex pendingCreationLock;
    WindowThread* pendingCreation = nullptr;

    LRESULT MessageProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowThread* parentThread = reinterpret_cast<WindowThread*>(GetWindowLongPtrA(handle, GWLP_USERDATA));
        if (parentThread != nullptr)
        {
            return parentThread->ProcessMessage(message, wParam, lParam);
        }

        if (pendingCreation != nullptr)
        {
            pendingCreation->windowHandle = handle;
            return pendingCreation->ProcessMessage(message, wParam, lParam);
        }

        Log(PlatformWindowsLog, LC_Error, "Window message without window handler: %i", message);
        return DefWindowProcA(handle, message, wParam, lParam);
    }

    LRESULT WindowThread::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CLOSE:
            {
                //TODO dispatch onWindowCloseRequest
                break;
            }
            case WM_DESTROY:
            {
                Log(PlatformWindowsLog, LC_Trace, "Window closed");
                //TODO dispatch onWindowClose
                RequestStop();
                break;
            }
        }

        return DefWindowProcA(windowHandle, message, wParam, lParam);
    }

    void WindowThread::SetTitle(String title)
    {
        if (windowHandle != 0)
        {
            SetWindowTextA(windowHandle, title.Data());
        }
    }

    void WindowThread::ThreadInit()
    {
        Log(PlatformWindowsLog, LC_Trace, "Creating window");

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

        RegisterClassExA(&windowThread);

        pendingCreationLock.Lock();
        pendingCreation = this;

        windowHandle = CreateWindowExA(0, WindowClass, nullptr, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, nullptr, nullptr, processHandle, nullptr);

        SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        pendingCreation = nullptr;
        pendingCreationLock.Unlock();

        Log(PlatformWindowsLog, LC_Trace, "Created window: %i", windowHandle);
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

        Sleep(Duration::FromMilliseconds(10));
    }

    void WindowThread::ThreadDestroy()
    {
        Log(PlatformWindowsLog, LC_Trace, "Destroying window: %i", windowHandle);

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
        windowThread.AddTask([title, this]()
        {
            windowThread.SetTitle(title);
        });
    }

    void WindowWindows::SetSize(Int2D size)
    {
    }

    Int2D WindowWindows::GetSize()
    {
        return {};
    }
}
