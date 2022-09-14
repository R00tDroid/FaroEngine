#include "MemoryManager.hpp"

void* Faro::MemoryManager::Alloc(uint32 size)
{
    return malloc(size);
}

void Faro::MemoryManager::Free(void* address)
{
    free(address);
}
