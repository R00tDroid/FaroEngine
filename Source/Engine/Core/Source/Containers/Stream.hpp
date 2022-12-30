#pragma once
#include <Primitives.hpp>

namespace Faro
{
    enum EStreamSeekOrigin
    {
        SSO_Start,
        SSO_End,
        SSO_Current
    };

    class DataStream
    {
        virtual uint32 Read(void* destination, uint16 elementSize, uint32 elementCount) = 0;
        virtual uint32 Write(void* source, uint16 elementSize, uint32 elementCount) = 0;
        virtual uint32 Size() = 0;
        virtual uint32 Tell() = 0;
        virtual uint32 Seek(EStreamSeekOrigin origin, int32 offset) = 0;

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
