#pragma once
#include <Containers/Path.hpp>
#include <Containers/Stream.hpp>
#include <Containers/Map.hpp>
#include "Resource.hpp"
#include <Memory/MemoryManager.hpp>
#include <Assert.hpp>
#include <Thread.hpp>

namespace Faro
{
    class ResourcePackage;

    class ResourceManager
    {
    public:
        void Init();
        void Destroy();

        DataStream* GetFile(Path path);

        template<class T>
        T* GetResource(Path path)
        {
            DebugBuildAssert(RequireSubClass(IResource, T), "Expected subclass of IResource");

            if (resources.Contains(path))
            {
                return resources[path];
            }

            DataStream* stream = GetFile(path);
            if (stream != nullptr)
            {
                T* resource = MemoryManager::New<T>(stream);
                resources.Add(path, static_cast<IResource*>(resource));
                return resource;
            }

            return nullptr;
        }

        void AddPackage(ResourcePackage*);

    private:
        Map<Path, DataStream*> files;
        Map<Path, IResource*> resources;
        Array<ResourcePackage*> loadedPackages;

        class ResourceLoaderThread : public IThread
        {
        protected:
            void ThreadInit() override;
            void ThreadUpdate() override;
            void ThreadDestroy() override;

        public:
            String GetThreadId() override;
        };
        Array<ResourceLoaderThread*> loaderThreads;

        void SetLoaderThreadCount(uint16 count);
    };
    extern ResourceManager GResources;
}
