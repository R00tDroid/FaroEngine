#include "PlatformModule.h"
#include "Platform.h"

namespace Faro
{
    void PlatformModule::Load()
    {
        Array<Platform*> PlatformTypes = GetRegisteredPlatforms();
        if (!PlatformTypes.Empty())
        {
            GPlatform = PlatformTypes[0];
            GPlatform->Init();
        }
    }

    void PlatformModule::Unload()
    {
        if (GPlatform != nullptr) 
        {
            GPlatform->Destroy();
            GPlatform = nullptr;
        }
    }

    String PlatformModule::GetName()
    {
        return "Platform";
    }

    Array<String> PlatformModule::GetRuntimeDependencies()
    {
        return {};
    }
}
