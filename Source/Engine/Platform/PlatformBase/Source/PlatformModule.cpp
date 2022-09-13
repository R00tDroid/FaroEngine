#include "PlatformModule.h"

namespace Faro
{
    void PlatformModule::Load()
    {
    }

    void PlatformModule::Unload()
    {
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