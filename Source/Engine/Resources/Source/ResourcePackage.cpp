#include "ResourcePackage.hpp"
#include <Platform.hpp>

namespace Faro
{
    Array<ResourcePackage*> ResourcePackage::staticPackages;

    ResourcePackage::ResourcePackage(String inMountRoot, Path inMountPath, bool autoRegister) : mountRoot(inMountRoot), mountPath(inMountPath)
    {
        if (autoRegister) SelfRegister();
    }

    const Array<ResourcePackage*>& ResourcePackage::GetStaticPackages()
    {
        return staticPackages;
    }

    Path ResourcePackage::GetMountRoot()
    {
        return mountRoot + ":" + mountPath.Get();
    }

    void ResourcePackage::SelfRegister()
    {
        staticPackages.Add(this);
    }

    ResourceDirectoryPackage::ResourceDirectoryPackage(String mountRoot, Path mountPath, Path inDirectory) : ResourcePackage(mountRoot, mountPath, true), directory(inDirectory)
    {
    }

    void ResourceDirectoryPackage::LoadResources()
    {
       Array<Path> filePaths = GPlatform->FindFiles(directory, true, "*.*");
       for (Path& file : filePaths)
       {
           DataStream* fileStream = GPlatform->OpenFile(file, FO_Read);
           files.Add( GetMountRoot() + file.GetRelative(directory).Get(), fileStream);
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
