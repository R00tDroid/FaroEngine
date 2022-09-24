#include "ThreadSafety.hpp"

namespace Faro
{
    void Mutex::Lock()
    {
        mutex.lock();
    }

    void Mutex::Unlock()
    {
        mutex.unlock();
    }
}
