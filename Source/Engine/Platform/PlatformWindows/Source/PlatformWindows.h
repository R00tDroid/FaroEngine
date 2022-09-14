#pragma once
#include <Platform.h>

namespace Faro
{
    class PlatformWindows : public Platform
    {
    public:
        void Init() override;
        void Destroy() override;

        String GetName() override { return "Windows"; }
    };

    REGISTER_PLATFORM(PlatformWindows)
}
