#include "GraphicsFenceVK.hpp"
#include <GraphicsAdapterVK.hpp>

namespace Faro
{
    void GraphicsFenceVK::Init()
    {
        GraphicsFence::Init();
    }

    void GraphicsFenceVK::Destroy()
    {
        GraphicsFence::Destroy();
    }

    void GraphicsFenceVK::Trigger()
    {
    }

    bool GraphicsFenceVK::HasFinished()
    {
        return false;
    }

    void GraphicsFenceVK::WaitForFinish()
    {
        if (HasFinished()) return;
    }
}
