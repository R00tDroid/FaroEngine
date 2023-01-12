#include "ResourceManager.hpp"
#include "ResourcePackage.hpp"
#include "ResourcesModule.hpp"

namespace Faro
{
    ResourceManager GResources;

    void ResourceManager::Init()
    {
        Logger::Log(ResourceLog, LC_Trace, "Loading static packages");
        const Array<ResourcePackage*>& staticPackages = ResourcePackage::GetStaticPackages();
        for (ResourcePackage* package : staticPackages)
        {
            AddPackage(package);
        }
    }

    void ResourceManager::Destroy()
    {
        Logger::Log(ResourceLog, LC_Trace, "Unloading packages");
        for (ResourcePackage* package : loadedPackages)
        {
            package->UnloadResources();
        }
        loadedPackages.Clear();
        files.Clear();
    }

    DataStream* ResourceManager::GetFile(Path path)
    {
        if (files.Contains(path))
        {
            Logger::Log(ResourceLog, LC_Trace, "Open resource %s", path.Get().Data());
            return files[path]->OpenCopy();
        }
        return nullptr;
    }

    void ResourceManager::AddPackage(ResourcePackage* package)
    {
        Logger::Log(ResourceLog, LC_Trace, "Loading package");

        package->LoadResources();
        loadedPackages.Add(package);

        uint32 resourceCount = 0;

        for (auto& it : package->GetResources())
        {
            files.Add(it.first, it.second);
            resourceCount++;
        }

        Logger::Log(ResourceLog, LC_Debug, "Loaded package: %i", resourceCount);
    }
}
