#pragma once
#include "Containers/Map.hpp"
#include "Containers/Path.hpp"
#include "Containers/Stream.hpp"

namespace Faro
{
    class ResourcePackage
    {
    public:
        ResourcePackage(bool autoRegister = false);
        virtual ~ResourcePackage() = default;

        virtual void LoadResources() = 0;
        virtual void UnloadResources() = 0;
        virtual Map<Path, DataStream*> GetResources() = 0;

        static const Array<ResourcePackage*>& GetStaticPackages();

    protected:
        void SelfRegister();

    private:
        static Array<ResourcePackage*> staticPackages;
    };

    class ResourceDirectoryPackage : public ResourcePackage
    {
    public:
        ResourceDirectoryPackage();
        ResourceDirectoryPackage(Path directory);

        void LoadResources() override;
        void UnloadResources() override;
        Map<Path, DataStream*> GetResources() override;

    private:
        Path directory;
        Map<Path, DataStream*> files;
    };
}
