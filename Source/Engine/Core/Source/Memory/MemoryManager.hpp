#pragma once
#include <cstdlib>
#include "../Primitives.hpp"

namespace Faro
{
    class MemoryManager
    {
    public:
        template<class T, typename... Args>
        static T* New(Args... args)
        {
            T* obj = static_cast<T*>(Alloc(sizeof(T)));
            return new (obj) T(args...);
        }

        template<class T>
        static T* Alloc(uint32 amount)
        {
            return static_cast<T*>(Alloc(sizeof(T) * amount));
        }

        static void* Alloc(uint32 size);

        static void Free(void* address);

        template<class T>
        static void Delete(T* address)
        {
            address->~T();
            Free(address);
        }
    };
}
