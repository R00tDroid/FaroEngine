#include "GraphicsInterface.hpp"

namespace Faro
{
    IMPLEMENT_INSTANCE_REGISTRY(GraphicsInterface, Faro::GraphicsInterface)

    GraphicsInterface* GGraphics = nullptr;

    Array<GraphicsInterface*> GetGraphicsInterfaces()
    {
        return InstanceRegistry_GraphicsInterface::Get().GetValues();
    }
}
