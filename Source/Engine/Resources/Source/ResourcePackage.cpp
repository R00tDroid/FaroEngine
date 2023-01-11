#include "ResourcePackage.hpp"
#include <Platform.hpp>

namespace Faro
{
    Array<ResourcePackage*> ResourcePackage::staticPackages;

    ResourcePackage::ResourcePackage(bool autoRegister)
    {
        if (autoRegister) SelfRegister();
    }

    const Array<ResourcePackage*>& ResourcePackage::GetStaticPackages()
    {
        return staticPackages;
    }

    void ResourcePackage::SelfRegister()
    {
        staticPackages.Add(this);
    }

    ResourceDirectoryPackage::ResourceDirectoryPackage()
    {
    }

    ResourceDirectoryPackage::ResourceDirectoryPackage(Path inDirectory) : ResourcePackage(true), directory(inDirectory)
    {
    }

    void ResourceDirectoryPackage::LoadResources()
    {
       Array<Path> filePaths = GPlatform->FindFiles(directory, true, "*.*");
       for (Path& file : filePaths)
       {
           DataStream* fileStream = GPlatform->OpenFile(file, FO_Read);
           files.Add(file.GetRelative(directory), fileStream);
       }
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
