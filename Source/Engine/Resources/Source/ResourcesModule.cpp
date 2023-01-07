#include "ResourcesModule.hpp"
#include <Log.hpp>

namespace Faro
{
    LOG_DECLARATION(ResourceLog, Resources)

    void ResourcesModule::Load()
    {
        Logger::Log(ResourceLog, LC_Debug, "Loading GResources");
    }

    void ResourcesModule::Unload()
    {
    }

    String ResourcesModule::GetName()
    {
        return "Resources";
    }

    Array<String> ResourcesModule::GetRuntimeDependencies()
    {
        return { "PlatformBase" };
    }
}
