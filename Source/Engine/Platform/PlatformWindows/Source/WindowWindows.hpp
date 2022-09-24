#pragma once
#include <Window.hpp>
#include <Thread.hpp>

namespace Faro
{
    class WindowThread : public IThread
    {
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
