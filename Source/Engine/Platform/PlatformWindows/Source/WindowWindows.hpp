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

        HWND windowHandle = nullptr;

        WindowWindows* window = nullptr;

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

    private:
        WindowThread windowThread;

        WindowState windowState;
    };
}
