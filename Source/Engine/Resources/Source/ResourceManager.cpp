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

        Logger::Log(ResourceLog, LC_Trace, "Starting default loader threads");
        SetLoaderThreadCount(4);
    }

    void ResourceManager::Destroy()
    {
        Logger::Log(ResourceLog, LC_Trace, "Stopping loader threads");
        SetLoaderThreadCount(0);

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

    void ResourceManager::ResourceLoaderThread::ThreadInit()
    {
        Logger::Log(ResourceLog, LC_Trace, "Loader thread started");
    }

    void ResourceManager::ResourceLoaderThread::ThreadUpdate()
    {
    }

    void ResourceManager::ResourceLoaderThread::ThreadDestroy()
    {
        Logger::Log(ResourceLog, LC_Trace, "Loader thread stopped");
    }

    String ResourceManager::ResourceLoaderThread::GetThreadId()
    {
        return "ResourceLoader";
    }

    void ResourceManager::SetLoaderThreadCount(uint16 count)
    {
        if (count > loaderThreads.Size())
        {
            while (count > loaderThreads.Size())
            {
                ResourceLoaderThread* thread = MemoryManager::New<ResourceLoaderThread>();
                thread->Start();
                loaderThreads.Add(thread);
            }
        }

        if (count < loaderThreads.Size())
        {
            while (count < loaderThreads.Size())
            {
                ResourceLoaderThread* thread = loaderThreads.First();
                thread->RequestStop();
                thread->WaitForStop();
                MemoryManager::Delete(thread);
                loaderThreads.Remove(thread);
            }
        }
    }
}
