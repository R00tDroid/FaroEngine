#pragma once
#include <Platform.h>

namespace Faro
{
    class PlatformWindows : public Platform
    {
    public:
        void Init() override;
        void Destroy() override;
    };

    REGISTER_PLATFORM(PlatformWindows)
}
