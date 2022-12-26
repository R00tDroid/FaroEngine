#include <GraphicsBufferContainer.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    void GraphicsBufferContainer::Init(GraphicsAdapter* adapter, GraphicsBufferType inType, GraphicsBufferDesc inDesc)
    {
        IGraphicsAdapterChild::Init(adapter);
        type = inType;
        desc = inDesc;

        resources[0] = GetAdapter()->CreateBuffer(type, desc);
        resources[1] = GetAdapter()->CreateBuffer(type, desc);
    }

    void GraphicsBufferContainer::Destroy()
    {
        MemoryManager::SafeDelete(resources[0]);
        MemoryManager::SafeDelete(resources[1]);
    }

    GraphicsBuffer* GraphicsBufferContainer::Get()
    {
        return resources[0]; //TODO use frame index
    }
}
