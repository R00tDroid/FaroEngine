#pragma once
#include <Util/ClassRegistry.hpp>
#include <Window.hpp>
#include <Monitor.hpp>
#include <FileStream.hpp>
#include <Containers/Path.hpp>

namespace Faro
{
    class Platform
    {
    public:
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual String GetName() = 0;

        virtual Window* CreateWindow() = 0;

        virtual Array<Monitor> GetMonitors() = 0;

        Monitor GetPrimaryMonitor();

        virtual FileStream* OpenFile(Path file) = 0;
    };

    extern Platform* GPlatform;

    extern Array<Platform*> GetRegisteredPlatforms();

    DEFINE_INSTANCE_REGISTRY(Platform, Faro::Platform)
#define REGISTER_PLATFORM(TYPE) REGISTER_INSTANCE(Platform, TYPE)
}
