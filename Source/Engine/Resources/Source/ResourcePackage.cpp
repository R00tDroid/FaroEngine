#include "ResourcePackage.hpp"
#include <Platform.hpp>

namespace Faro
{
    ResourcePackage::ResourcePackage(bool autoRegister)
    {
        if (autoRegister) SelfRegister();
    }

    void ResourcePackage::SelfRegister()
    {
        //TODO Register with resource manager
    }

    void ResourceDirectoryPackage::LoadResources()
    {
        //TODO Find and open files
    }

    void ResourceDirectoryPackage::UnloadResources()
    {
        for (auto& it : files)
        {
            it.second->Close();
        }
        files.Clear();
    }

    Map<Path, DataStream*> ResourceDirectoryPackage::GetResources()
    {
        return files;
    }
}
