#pragma once
#include <Containers/String.hpp>
#include <Math/Vector2.hpp>
#include <Memory/Object.hpp>

namespace Faro
{
    class Window : public IObject
    {
    public:
        virtual ~Window() = default;

        virtual void SetTitle(String title) = 0;

        virtual void SetSize(Int2D size) = 0;

        virtual Int2D GetSize() = 0;
    };

}
