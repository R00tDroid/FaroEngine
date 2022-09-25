#pragma once
#include <Window.hpp>
#include <Thread.hpp>
#include <MinWindows.hpp>

namespace Faro
{
    class WindowThread : public IThread
    {
    public:
        LRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

        void SetTitle(String title);

    protected:
        void ThreadInit() override;
        void ThreadUpdate() override;
        void ThreadDestroy() override;

    private:
        HWND windowHandle = 0;
    };

    class WindowWindows : public Window
    {
    public:
        void Init() override;
        void Destroy() override;

        void SetTitle(String title) override;
        void SetSize(Int2D size) override;
        Int2D GetSize() override;

    private:
        struct WindowState
        {
            Int2D position = Int2D::zero;
            Int2D size = Int2D::zero;
        };

        WindowState windowStates[WindowMode::WM_ENUMSIZE];

        WindowThread windowThread;
    };
}
