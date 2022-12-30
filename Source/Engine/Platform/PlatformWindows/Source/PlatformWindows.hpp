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

        Window* CreateWindow() override;

        Array<Monitor> GetMonitors() override;

        FileStream* OpenFile(Path file, EFileMode mode) override;
    };

    REGISTER_PLATFORM(PlatformWindows)
}
