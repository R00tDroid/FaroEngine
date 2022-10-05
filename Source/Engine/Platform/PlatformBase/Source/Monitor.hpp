#pragma once
#include <Containers/String.hpp>
#include <Math/Rect.hpp>

namespace Faro
{
    struct Monitor
    {
        IntRect desktop;
        String identifier;

        inline bool IsValid() const
        {
            return desktop.Area() > 0;
        }
    };
}
