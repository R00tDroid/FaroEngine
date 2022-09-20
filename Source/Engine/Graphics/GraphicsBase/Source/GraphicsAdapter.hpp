#pragma once
#include <Containers/String.hpp>

namespace Faro
{
    struct GraphicsAdapterDesc
    {
        String name;
        String manufacturer;

        uint32 vram = 0;

        void* payload = nullptr;
    };
}
