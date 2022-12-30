#pragma once
#include <Containers/Stream.hpp>

namespace Faro
{
    enum EFileMode
    {
        FO_Overwrite,
        FO_Append,
        FO_Read
    };

    class FileStream : public DataStream
    {
    public:
        virtual bool IsOpen() = 0;
    };
}
