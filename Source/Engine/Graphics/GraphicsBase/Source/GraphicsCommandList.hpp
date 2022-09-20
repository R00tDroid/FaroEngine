#pragma once
#include "GraphicsAdapterChild.hpp"

namespace Faro
{
    class GraphicsCommandList : public IGraphicsAdapterChild
    {
    public:
        virtual void Reset() = 0;
        virtual void Execute() = 0;
    };
}
