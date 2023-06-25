#include "GraphicsInterfaceVK.hpp"
#include <ShaderDefinition.hpp>

namespace Faro
{
    bool GraphicsInterfaceVK::Init()
    {
        return true;
    }

    void GraphicsInterfaceVK::Destroy()
    {
    }

    Array<GraphicsAdapterDesc> GraphicsInterfaceVK::GetAdapters()
    {
        return {};
    }

    GraphicsAdapter* GraphicsInterfaceVK::CreateAdapter(GraphicsAdapterDesc description)
    {
        return nullptr;
    }

    uint8 GraphicsInterfaceVK::GetShaderBinaryType()
    {
        return ShaderBinaryType_SPRIV;
    }
}
