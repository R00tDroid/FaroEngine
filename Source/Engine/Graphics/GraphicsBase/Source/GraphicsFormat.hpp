#pragma once
#include <Primitives.hpp>

namespace Faro
{
    enum GraphicsFormatElements
    {
        FE_Unknown,
        FE_R,
        FE_RG,
        FE_RGB,
        FE_RGBA,
        FE_BGRA,
    };

    enum GraphicsFormatElementType
    {
        FET_Unknown,
        FET_Float,
        FET_UNorm,
        FET_SNorm,
        FET_UInt,
        FET_SInt
    };

    enum GraphicsFormatElementLayout
    {
        FEL_Unknown,
        FEL_8,
        FEL_8_8,
        FEL_8_8_8,
        FEL_8_8_8_8,
        FEL_16,
        FEL_16_16,
        FEL_16_16_16,
        FEL_16_16_16_16,
        FEL_32,
        FEL_32_32,
        FEL_32_32_32,
        FEL_32_32_32_32,
    };

    struct GraphicsFormat
    {
        GraphicsFormatElements elements = FE_Unknown;
        GraphicsFormatElementType elementType = FET_Unknown;
        GraphicsFormatElementLayout elementLayout = FEL_Unknown;
        bool linear = true;

        uint8 GetElementCount();
        uint8 GetStride();

        static GraphicsFormat Byte;
        static GraphicsFormat Byte2;
        static GraphicsFormat Byte4;

        static GraphicsFormat Float;
        static GraphicsFormat Float2;
        static GraphicsFormat Float3;
        static GraphicsFormat Float4;
    };
}
