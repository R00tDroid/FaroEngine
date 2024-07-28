#pragma once
#if FARO_OS_ANDROID
#include <Platform.hpp>

namespace Faro
{
    class PlatformAndroid : public Platform
    {
    public:
        void Init() override;
        void Destroy() override;

        String GetName() override { return "Android"; }

        Window* CreateWindow() override;

        Array<Monitor> GetMonitors() override;

        FileStream* OpenFile(Path file, EFileMode mode) override;

        Array<Path> FindSubPaths(Path rootPath, bool recursive, String filter) override;
    };

    REGISTER_PLATFORM(PlatformAndroid)
}
#endif
