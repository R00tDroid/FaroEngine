#pragma once
#include <Primitives.hpp>

namespace Faro
{
    class DataStream
    {
        virtual uint32 Read(void* destination, uint16 elementSize, uint32 elementCount) = 0;

        template<typename T>
        uint32 Read(T* destination, uint32 elementCount)
        {
            return Read(destination, sizeof(T), elementCount) / sizeof(T);
        }

        template<typename T>
        T Read()
        {
            T returnValue;
            Read<T>(&returnValue, 1);
            return returnValue;
        }

        virtual uint32 Write(void* source, uint16 elementSize, uint32 elementCount) = 0;

        template<typename T>
        uint32 Write(T* source, uint32 elementCount)
        {
            return Write(source, sizeof(T), elementCount);
        }

        template<typename T>
        void Write(T value)
        {
            Write<T>(&value, 1);
        }
    };
}
