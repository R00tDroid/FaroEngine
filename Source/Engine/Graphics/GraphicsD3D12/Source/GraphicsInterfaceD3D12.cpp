#include "GraphicsInterfaceD3D12.hpp"

namespace Faro
{
    bool GraphicsInterfaceD3D12::Init()
    {
        return true;
    }

    void GraphicsInterfaceD3D12::Destroy()
    {
    }

    Array<GraphicsAdapterDesc> GraphicsInterfaceD3D12::GetAdapters()
    {
        return {};
    }
}
