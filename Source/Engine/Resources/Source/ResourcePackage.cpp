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
