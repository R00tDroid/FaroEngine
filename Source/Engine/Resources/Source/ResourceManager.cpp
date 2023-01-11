#include "ResourceManager.hpp"
#include "ResourcePackage.hpp"

namespace Faro
{
    ResourceManager GResources;

    void ResourceManager::Init()
    {
        const Array<ResourcePackage*>& staticPackages = ResourcePackage::GetStaticPackages();
        for (ResourcePackage* package : staticPackages)
        {
            AddPackage(package);
        }
    }

    void ResourceManager::Destroy()
    {
        for (ResourcePackage* package : loadedPackages)
        {
            package->UnloadResources();
        }
        loadedPackages.Clear();
        files.Clear();
    }

    DataStream* ResourceManager::GetFile(Path)
    {
        return nullptr; //TODO Find file in packages
    }

    void ResourceManager::AddPackage(ResourcePackage* package)
    {
        package->LoadResources();
        loadedPackages.Add(package);

        for (auto& it : package->GetResources())
        {
            files.Add(it.first, it.second);
        }
    }
}
