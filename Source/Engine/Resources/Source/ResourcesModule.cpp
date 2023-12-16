#include "ResourcesModule.hpp"
#include "ResourceManager.hpp"

namespace Faro
{
    REGISTER_MODULE(ResourcesModule)

    LOG_DECLARATION(ResourceLog, Resources)

    void ResourcesModule::Load()
    {
        Logger::Log(ResourceLog, LC_Debug, "Loading GResources");
        GResources.Init();
    }

    void ResourcesModule::Unload()
    {
        GResources.Destroy();
    }

    String ResourcesModule::GetName()
    {
        return "Resources";
    }

    Array<String> ResourcesModule::GetRuntimeDependencies()
    {
        return { "Platform" };
    }
}
