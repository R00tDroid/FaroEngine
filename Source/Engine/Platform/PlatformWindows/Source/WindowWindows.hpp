#pragma once
#include <Window.hpp>
#include <Thread.hpp>
#include <MinWindows.hpp>

namespace Faro
{
    class WindowWindows;

    class WindowThread : public IThread
    {
    public:
        LRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

        void SetTitle(String title);
        void SetState(WindowState state);

        HWND windowHandle = nullptr;

        WindowWindows* window = nullptr;

        ThreadSafe<WindowState> windowState;

    protected:
        void ThreadInit() override;
        void ThreadUpdate() override;
        void ThreadDestroy() override;
    };

    class WindowWindows : public Window
    {
    public:
        void Init() override;
        void Destroy() override;

        void SetTitle(String title) override;

        void SetWindowState(WindowState info) override;
        WindowState GetWindowState() override;

        void NotifyStateChange();

    private:
        WindowThread windowThread;

        WindowState windowState;
    };
}
