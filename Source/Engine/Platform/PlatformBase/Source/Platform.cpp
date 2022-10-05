#include "Platform.hpp"

namespace Faro
{
    IMPLEMENT_INSTANCE_REGISTRY(Platform, Faro::Platform)

    Platform* GPlatform = nullptr;

    Monitor Platform::GetPrimaryMonitor()
    {
        for (Monitor& monitor : GetMonitors())
        {
            if (monitor.desktop.topLeft == Int2D::zero)
            {
                return monitor;
            }
        }
        return Monitor();
    }

    Array<Platform*> GetRegisteredPlatforms()
    {
        return InstanceRegistry_Platform::Get().GetValues();
    }
}
