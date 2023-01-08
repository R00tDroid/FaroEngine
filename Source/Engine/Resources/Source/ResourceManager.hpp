#pragma once
#include <Containers/Path.hpp>
#include <Containers/Stream.hpp>
#include <Containers/Map.hpp>
#include "Resource.hpp"
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    class ResourceManager
    {
    public:
        void Init();
        void Destroy();

        DataStream* GetFile(Path path);

        template<class T>
        T* GetResource(Path path)
        {
            if (resources.Contains(path))
            {
                return resources[path];
            }

            DataStream* stream = GetFile(path);
            if (stream != nullptr)
            {
                T* resource = MemoryManager::New<T>(); //TODO Init resource with stream
                resources.Add(path, static_cast<IResource*>(resource));
                return resource;
            }

            return nullptr;
        }

    private:
        Map<Path, IResource*> resources;
    };
    extern ResourceManager GResources;
}
