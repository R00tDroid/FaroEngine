#pragma once
#include <Containers/String.hpp>
#include <Math/Vector2.hpp>
#include <Memory/Object.hpp>

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

    class Window : public IObject
    {
    public:
        virtual ~Window() = default;

        virtual void SetTitle(String title) = 0;

        virtual void SetSize(Int2D size) = 0;

        virtual Int2D GetSize() = 0;
    };
}
