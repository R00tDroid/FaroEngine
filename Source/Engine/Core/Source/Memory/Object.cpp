#include "Object.hpp"

#include "MemoryManager.hpp"

namespace Faro
{
    void IObject::Init()
    {
    }

    void IObject::Destroy()
    {
        MemoryManager::Delete(this);
    }
}

