#pragma once
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    class GraphicsSyncPoint : public IGraphicsAdapterChild
    {
    public:
        using IGraphicsAdapterChild::Init;
    };
}
