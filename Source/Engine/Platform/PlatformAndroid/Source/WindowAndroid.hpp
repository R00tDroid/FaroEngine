#pragma once
#if FARO_OS_ANDROID
#include <Window.hpp>
#include <Thread.hpp>

namespace Faro
{
    class WindowAndroid;

    class WindowAndroid : public Window
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
