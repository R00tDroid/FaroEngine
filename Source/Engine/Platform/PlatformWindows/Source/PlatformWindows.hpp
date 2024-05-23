#pragma once
#if FARO_PLATFORM_WINDOWS
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

        Array<Path> FindSubPaths(Path rootPath, bool recursive, String filter) override;
    };

    REGISTER_PLATFORM(PlatformWindows)
}
#endif
