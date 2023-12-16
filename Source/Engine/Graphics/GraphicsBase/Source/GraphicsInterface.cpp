#include "GraphicsInterface.hpp"

namespace Faro
{
    GraphicsInterface* GGraphics = nullptr;

    Array<GraphicsInterface*> GetGraphicsInterfaces()
    {
        return {};// InstanceRegistry_GraphicsInterface::Get().GetValues();
    }
}
