#pragma once
#include <functional>

namespace Faro
{
    typedef uint8_t uint8;
    typedef int8_t int8;

    typedef uint16_t uint16;
    typedef int16_t int16;

    typedef uint32_t uint32;
    typedef int32_t int32;

    typedef uint64_t uint64;
    typedef int64_t int64;

    template <typename T>
    using Function = std::function<T>;
}
