#pragma once
#include <Platform.hpp>

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
