#pragma once
#include "Containers/Map.hpp"
#include "Containers/Path.hpp"
#include "Containers/Stream.hpp"

namespace Faro
{
#define ResourceMount_Engine "Engine"
#define ResourceMount_Game "Game"

    class ResourcePackage
    {
    public:
        ResourcePackage(String mountRoot, Path mountPath = String(), bool autoRegister = false);
        virtual ~ResourcePackage() = default;

        virtual void LoadResources() = 0;
        virtual void UnloadResources() = 0;
        virtual Map<Path, DataStream*> GetResources() = 0;

        static const Array<ResourcePackage*>& GetStaticPackages();

        Path GetMountRoot();

    protected:
        void SelfRegister();

    private:
        static Array<ResourcePackage*> staticPackages;

        String mountRoot;
        Path mountPath;
    };

    class ResourceDirectoryPackage : public ResourcePackage
    {
    public:
        ResourceDirectoryPackage(String mountRoot, Path mountPath, Path directory);

        void LoadResources() override;
        void UnloadResources() override;
        Map<Path, DataStream*> GetResources() override;

    private:
        Path directory;
        Map<Path, DataStream*> files;
    };
}
