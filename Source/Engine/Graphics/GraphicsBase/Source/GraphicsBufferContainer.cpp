#include <GraphicsBufferContainer.hpp>
#include <GraphicsAdapter.hpp>

namespace Faro
{
    void GraphicsBufferContainer::Init(GraphicsAdapter* adapter, GraphicsBufferCreateDesc inCreateDesc)
    {
        IGraphicsAdapterChild::Init(adapter);
        createDesc = inCreateDesc;

        resources[0] = GetAdapter()->CreateBuffer(createDesc);
        resources[1] = GetAdapter()->CreateBuffer(createDesc);
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
