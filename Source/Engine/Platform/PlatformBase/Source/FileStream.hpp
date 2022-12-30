#pragma once
#include <Containers/Stream.hpp>

namespace Faro
{
    class FileStream : public DataStream
    {
    public:
        virtual bool IsOpen() = 0;
    };
}
