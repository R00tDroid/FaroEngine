#include "GraphicsAdapter.hpp"

namespace Faro
{
    GraphicsSemaphore* GraphicsAdapter::CreateSyncPoint()
    {
        GraphicsSemaphore* syncPoint = MemoryManager::New<GraphicsSemaphore>();
        syncPoint->Init(this);
        return syncPoint;
    }
}
