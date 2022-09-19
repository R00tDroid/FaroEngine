#pragma once
#include <Containers/Array.hpp>
#include <Util/ClassRegistry.hpp>

namespace Faro
{
    class GraphicsInterface
    {
        
    };

    extern GraphicsInterface* GGraphics;

    extern Array<GraphicsInterface*> GetGraphicsInterfaces();

    DEFINE_INSTANCE_REGISTRY(GraphicsInterface, Faro::GraphicsInterface)
#define REGISTER_GRAPHICS(TYPE) REGISTER_INSTANCE(Platform, TYPE)
}
