#pragma once
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include "Object.hpp"
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

        template<class T, typename... Args>
        static T* NewArgList(va_list args)
        {
            T* obj = static_cast<T*>(Alloc(sizeof(T)));
            return new (obj) T(args);
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

        template<class T>
        static void Set(T* address, uint8 value, uint32 amount)
        {
            std::memset((void*)address, value, sizeof(T) * amount);
        }

        template<class T>
        static void Zero(T* address, uint32 amount = 1)
        {
            Set<T>(address, 0, amount);
        }

        template<class T>
        static void SafeDelete(T*& objectPtr)
        {
            if (objectPtr != nullptr)
            {
                objectPtr->Destroy();
                objectPtr = nullptr;
            }
        }

        template<class T>
        static void Copy(T* source, T* destination, uint32 elementCount = 1)
        {
            memcpy((void*)destination, (void*)source, sizeof(T) * elementCount);
        }
    };
}
