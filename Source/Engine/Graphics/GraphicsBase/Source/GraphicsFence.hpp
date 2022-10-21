#pragma once
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    class GraphicsFence : public IGraphicsAdapterChild
    {
    public:
        virtual void Trigger() = 0;

        virtual bool HasFinished() = 0;
        virtual void WaitForFinish()  =0;
    };
}
