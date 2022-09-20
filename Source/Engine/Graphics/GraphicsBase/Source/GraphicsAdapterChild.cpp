#include "GraphicsAdapterChild.hpp"

namespace Faro
{
    void IGraphicsAdapterChild::Init(GraphicsAdapter* inAdapter)
    {
        adapter = inAdapter;
        Init();
    }

    GraphicsAdapter* IGraphicsAdapterChild::GetAdapter()
    {
        return adapter;
    }

    void IGraphicsAdapterChild::Init()
    {
        IObject::Init();
    }
}
