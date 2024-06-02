#pragma once
#if FARO_OS_WEB
#include <Window.hpp>
#include <Thread.hpp>

namespace Faro
{
    class WindowWeb;

    class WindowWeb : public Window
    {
    public:
        void Init() override;
        void Destroy() override;

        void SetTitle(String title) override;

        void SetWindowState(WindowState info) override;
        WindowState GetWindowState() override;
    };
}
#endif
