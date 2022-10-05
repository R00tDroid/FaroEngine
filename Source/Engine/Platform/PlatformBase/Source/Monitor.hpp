#pragma once
#include <Containers/String.hpp>
#include <Math/Rect.hpp>

namespace Faro
{
    struct Monitor
    {
        IntRect desktop;
        String identifier;
    };
}
