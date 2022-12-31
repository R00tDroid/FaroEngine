#include "ConversionD3D12.hpp"
#include <Containers/Map.hpp>

namespace Faro
{
    Map<DXGI_FORMAT, GraphicsFormat> conversionA;
    Map<GraphicsFormat, DXGI_FORMAT> conversionB;

#define RegisterMapping(A, B) conversionA.Add(A, B); conversionB.Add(B, A);

void LoadConversionMaps()
    {
        if (conversionA.Size() == 0)
        {
            RegisterMapping(DXGI_FORMAT_R8_SINT, GraphicsFormat(FE_R, FET_SInt, FEL_8));
            RegisterMapping(DXGI_FORMAT_R8_UINT, GraphicsFormat(FE_R, FET_UInt, FEL_8));
            RegisterMapping(DXGI_FORMAT_R8_SNORM, GraphicsFormat(FE_R, FET_SNorm, FEL_8));
            RegisterMapping(DXGI_FORMAT_R8_UNORM, GraphicsFormat(FE_R, FET_UNorm, FEL_8));

            RegisterMapping(DXGI_FORMAT_R8G8_SINT, GraphicsFormat(FE_RG, FET_SInt, FEL_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8_UINT, GraphicsFormat(FE_RG, FET_UInt, FEL_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8_SNORM, GraphicsFormat(FE_RG, FET_SNorm, FEL_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8_UNORM, GraphicsFormat(FE_RG, FET_UNorm, FEL_8_8));

            RegisterMapping(DXGI_FORMAT_R8G8B8A8_SINT, GraphicsFormat(FE_RGBA, FET_SInt, FEL_8_8_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8B8A8_UINT, GraphicsFormat(FE_RGBA, FET_UInt, FEL_8_8_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8B8A8_SNORM, GraphicsFormat(FE_RGBA, FET_SNorm, FEL_8_8_8_8));
            RegisterMapping(DXGI_FORMAT_R8G8B8A8_UNORM, GraphicsFormat(FE_RGBA, FET_UNorm, FEL_8_8_8_8));

            RegisterMapping(DXGI_FORMAT_R16_SINT, GraphicsFormat(FE_R, FET_SInt, FEL_16));
            RegisterMapping(DXGI_FORMAT_R16_UINT, GraphicsFormat(FE_R, FET_UInt, FEL_16));
            RegisterMapping(DXGI_FORMAT_R16_SNORM, GraphicsFormat(FE_R, FET_SNorm, FEL_16));
            RegisterMapping(DXGI_FORMAT_R16_UNORM, GraphicsFormat(FE_R, FET_UNorm, FEL_16));

            RegisterMapping(DXGI_FORMAT_R16G16_SINT, GraphicsFormat(FE_RG, FET_SInt, FEL_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16_UINT, GraphicsFormat(FE_RG, FET_UInt, FEL_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16_SNORM, GraphicsFormat(FE_RG, FET_SNorm, FEL_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16_UNORM, GraphicsFormat(FE_RG, FET_UNorm, FEL_16_16));

            RegisterMapping(DXGI_FORMAT_R16G16B16A16_SINT, GraphicsFormat(FE_RGBA, FET_SInt, FEL_16_16_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16B16A16_UINT, GraphicsFormat(FE_RGBA, FET_UInt, FEL_16_16_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16B16A16_SNORM, GraphicsFormat(FE_RGBA, FET_SNorm, FEL_16_16_16_16));
            RegisterMapping(DXGI_FORMAT_R16G16B16A16_UNORM, GraphicsFormat(FE_RGBA, FET_UNorm, FEL_16_16_16_16));

            RegisterMapping(DXGI_FORMAT_R32_SINT, GraphicsFormat(FE_R, FET_SInt, FEL_32));
            RegisterMapping(DXGI_FORMAT_R32_UINT, GraphicsFormat(FE_R, FET_UInt, FEL_32));
            RegisterMapping(DXGI_FORMAT_R32_FLOAT, GraphicsFormat(FE_R, FET_Float, FEL_32));

            RegisterMapping(DXGI_FORMAT_R32G32_SINT, GraphicsFormat(FE_RG, FET_SInt, FEL_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32_UINT, GraphicsFormat(FE_RG, FET_UInt, FEL_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32_FLOAT, GraphicsFormat(FE_RG, FET_Float, FEL_32_32));

            RegisterMapping(DXGI_FORMAT_R32G32B32_SINT, GraphicsFormat(FE_RGB, FET_SInt, FEL_32_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32B32_UINT, GraphicsFormat(FE_RGB, FET_UInt, FEL_32_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32B32_FLOAT, GraphicsFormat(FE_RGB, FET_Float, FEL_32_32_32));

            RegisterMapping(DXGI_FORMAT_R32G32B32A32_SINT, GraphicsFormat(FE_RGBA, FET_SInt, FEL_32_32_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32B32A32_UINT, GraphicsFormat(FE_RGBA, FET_UInt, FEL_32_32_32_32));
            RegisterMapping(DXGI_FORMAT_R32G32B32A32_FLOAT, GraphicsFormat(FE_RGBA, FET_Float, FEL_32_32_32_32));
        }
    }

    DXGI_FORMAT Convert(GraphicsFormat format)
    {
        LoadConversionMaps();
        if (conversionB.Contains(format))
        {
            return conversionB[format];
        }
        else
        {
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    GraphicsFormat Convert(DXGI_FORMAT format)
    {
        LoadConversionMaps();
        if (conversionA.Contains(format))
        {
            return conversionA[format];
        }
        else
        {
            return {};
        }
    }
}
