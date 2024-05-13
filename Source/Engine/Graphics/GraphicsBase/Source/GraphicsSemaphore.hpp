#pragma once
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    class GraphicsSemaphore : public IGraphicsAdapterChild
    {
    public:
        using IGraphicsAdapterChild::Init;
    };
}
