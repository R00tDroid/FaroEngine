#include "GraphicsAdapter.hpp"

namespace Faro
{
    GraphicsSyncPoint* GraphicsAdapter::CreateSyncPoint()
    {
        GraphicsSyncPoint* syncPoint = MemoryManager::New<GraphicsSyncPoint>();
        syncPoint->Init(this);
        return syncPoint;
    }
}
