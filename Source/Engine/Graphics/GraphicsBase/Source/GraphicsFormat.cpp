#include "GraphicsFormat.hpp"

namespace Faro
{
    GraphicsFormat GraphicsFormat::Byte = { FE_R, FET_UNorm, FEL_8 };
    GraphicsFormat GraphicsFormat::Byte2 = { FE_RG, FET_UNorm, FEL_8_8 };
    GraphicsFormat GraphicsFormat::Byte4 = { FE_RGBA, FET_UNorm, FEL_8_8_8_8 };

    GraphicsFormat GraphicsFormat::Float = { FE_R, FET_Float, FEL_32 };
    GraphicsFormat GraphicsFormat::Float2 = { FE_RG, FET_Float, FEL_32_32 };
    GraphicsFormat GraphicsFormat::Float3 = { FE_RGB, FET_Float, FEL_32_32_32 };
    GraphicsFormat GraphicsFormat::Float4 = { FE_RGBA, FET_Float, FEL_32_32_32_32 };

    uint8 GraphicsFormat::GetElementCount()
    {
        switch (elements)
        {
            case FE_R: return 1;
            case FE_RG: return 2;
            case FE_RGB: return 3;
            case FE_RGBA: return 4;
            case FE_BGRA: return 4;
        }
        return 0;
    }

    uint8 GraphicsFormat::GetStride()
    {
        switch (elementLayout)
        {
            case FEL_8: return 1;
            case FEL_8_8: return 2;
            case FEL_8_8_8: return 3;
            case FEL_8_8_8_8: return 4;
            case FEL_16: return 2;
            case FEL_16_16: return 4;
            case FEL_16_16_16: return 6;
            case FEL_16_16_16_16: return 8;
            case FEL_32: return 4;
            case FEL_32_32: return 8;
            case FEL_32_32_32: return 12;
            case FEL_32_32_32_32: return 16;
        }
        return 0;
    }
}
