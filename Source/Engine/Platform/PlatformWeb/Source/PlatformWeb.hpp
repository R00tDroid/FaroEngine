#pragma once
#if FARO_OS_WEB
#include <Platform.hpp>

namespace Faro
{
    class PlatformWeb : public Platform
    {
    public:
        void Init() override;
        void Destroy() override;

        String GetName() override { return "Web"; }

        Window* CreateWindow() override;

        Array<Monitor> GetMonitors() override;

        FileStream* OpenFile(Path file, EFileMode mode) override;

        Array<Path> FindSubPaths(Path rootPath, bool recursive, String filter) override;
    };

    REGISTER_PLATFORM(PlatformWeb)
}
#endif
