#include "WindowWindows.hpp"
#include <MinWindows.hpp>
#include <MainThread.hpp>
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

        Logger::Log(PlatformWindowsLog, LC_Error, "Window message without window handler: %i", message);
        return DefWindowProcA(handle, message, wParam, lParam);
    }

    LRESULT WindowThread::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CLOSE:
            {
                bool closeWindow = true;
                window->onWindowCloseRequest.Dispatch(closeWindow);

                if (closeWindow)
                {
                    DestroyWindow(windowHandle);
                }

                return 0;
            }
            case WM_DESTROY:
            {
                Logger::Log(PlatformWindowsLog, LC_Trace, "Window closed");

                RequestStop();

                WindowWindows* windowPtr = window;
                RunOnThread(MainThread::id, [windowPtr]()
                {
                    windowPtr->onWindowClose.Dispatch();
                });

                return 0;
            }

            case WM_SIZE:
            case WM_MOVE:
            {
                RECT windowRect;
                GetWindowRect(windowHandle , &windowRect);

                RECT clientRect;
                GetClientRect(windowHandle, &clientRect);

                windowState.Lock();
                windowState->outerRect = { {windowRect.left, windowRect.top }, { windowRect.right - windowRect.left, windowRect.bottom - windowRect.top } };
                windowState->innerSize = { clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };
                windowState.Unlock();

                WindowWindows* windowPtr = window;
                RunOnThread(MainThread::id, [windowPtr]()
                {
                    windowPtr->NotifyStateChange();
                });
                return 0;
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

    void WindowThread::SetState(WindowState state)
    {
        //TODO resize window
    }

    void WindowThread::ThreadInit()
    {
        Logger::Log(PlatformWindowsLog, LC_Trace, "Creating window");

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

        if (windowHandle == nullptr)
        {
            Logger::Log(PlatformWindowsLog, LC_Error, "Failed to create window");
        }

        SetWindowLongPtrA(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        pendingCreation = nullptr;
        pendingCreationLock.Unlock();

        Logger::Log(PlatformWindowsLog, LC_Trace, "Created window: %i", windowHandle);
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
        Logger::Log(PlatformWindowsLog, LC_Trace, "Destroying window: %i", windowHandle);

        if (windowHandle != 0) 
        {
            DestroyWindow(windowHandle);
            windowHandle = 0;
        }
    }

    void WindowWindows::Init()
    {
        Window::Init();
        windowThread.window = this;
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

    void WindowWindows::SetWindowState(WindowState info)
    {
        windowState = info;

        windowThread.AddTask([info, this]()
        {
            windowThread.SetState(info);
        });
    }

    WindowState WindowWindows::GetWindowState()
    {
        return windowState;
    }

    void WindowWindows::NotifyStateChange()
    {
        windowState = windowThread.windowState.GetCopy();
        //TODO dispatch broadcast
    }
}
