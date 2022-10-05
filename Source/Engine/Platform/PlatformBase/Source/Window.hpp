#pragma once
#include <Containers/String.hpp>
#include <Math/Vector2.hpp>
#include <Memory/Object.hpp>
#include <Util/Broadcast.hpp>
#include <Math/Rect.hpp>

namespace Faro
{
    enum WindowMode
    {
        WM_Hidden,
        WM_Minimized,
        WM_Windowed,
        WM_Maximized,
        WM_Fullscreen,

        WM_ENUMSIZE
    };

    struct WindowState
    {
        WindowMode mode = WM_Hidden;
        IntRect outerRect;
        Int2D innerSize = Int2D::zero;
    };

    class Window : public IObject
    {
    public:
        virtual ~Window() = default;

        virtual void SetTitle(String title) = 0;

        virtual void SetWindowState(WindowState info) = 0;
        virtual WindowState GetWindowState() = 0;

        Broadcast<WindowState> onWindowStateChanged;

        Broadcast<> onWindowClose;

        Broadcast<bool&> onWindowCloseRequest;
    };
}
