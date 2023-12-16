#include "PlatformModule.hpp"
#include "Platform.hpp"
#include <Log.hpp>

namespace Faro
{
    REGISTER_MODULE(PlatformModule)

    LOG_DECLARATION(PlatformLog, Platform)

    void PlatformModule::Load()
    {
        Logger::Log(PlatformLog, LC_Debug, "Loading GPlatform");

        Array<Platform*> PlatformTypes = GetRegisteredPlatforms();
        if (!PlatformTypes.Empty())
        {
            GPlatform = PlatformTypes[0];
            Logger::Log(PlatformLog, LC_Info, "Platform: %s", GPlatform->GetName().Data());
            GPlatform->Init();
        }
        else
        {
            Logger::Log(PlatformLog, LC_Fatal, "Failed to find a suitable platform implementation");
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
