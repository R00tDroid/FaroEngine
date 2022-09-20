#pragma once
#include <Containers/Array.hpp>
#include <Util/ClassRegistry.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    class GraphicsInterface
    {
    public:
        virtual bool Init() = 0;
        virtual void Destroy() = 0;

        virtual Array<GraphicsAdapterDesc> GetAdapters() = 0;

        virtual GraphicsAdapter* CreateAdapter(GraphicsAdapterDesc description) = 0;

        virtual String GetName() = 0;
    };

    extern GraphicsInterface* GGraphics;

    extern Array<GraphicsInterface*> GetGraphicsInterfaces();

    DEFINE_INSTANCE_REGISTRY(GraphicsInterface, Faro::GraphicsInterface)
#define REGISTER_GRAPHICS(TYPE) REGISTER_INSTANCE(GraphicsInterface, TYPE)
}
