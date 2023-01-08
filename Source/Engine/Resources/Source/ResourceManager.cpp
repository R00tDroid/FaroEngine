#include "ResourceManager.hpp"

namespace Faro
{
    ResourceManager GResources;

    void ResourceManager::Init()
    {
    }

    void ResourceManager::Destroy()
    {
    }

    DataStream* ResourceManager::GetFile(Path)
    {
        return nullptr; //TODO Find file in packages
    }
}
