#include "Platform.h"

namespace Faro
{
    IMPLEMENT_INSTANCE_REGISTRY(Platform, Faro::Platform)

    Platform* GPlatform = nullptr;

    Array<Platform*> GetRegisteredPlatforms()
    {
        return InstanceRegistry_Platform::Get().GetValues();
    }
}