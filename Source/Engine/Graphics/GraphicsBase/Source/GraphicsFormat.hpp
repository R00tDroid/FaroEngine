#pragma once
#include <Primitives.hpp>

namespace Faro
{
    /// @brief Describes what elements are present in a format and what order they have in storage.
    enum GraphicsFormatElements
    {
        FE_Unknown,
        FE_R,
        FE_RG,
        FE_RGB,
        FE_RGBA,
        FE_BGRA,
    };

    /// @brief Describes the datatype of a format element.
    enum GraphicsFormatElementType
    {
        FET_Unknown,
        FET_Float,
        FET_UNorm,
        FET_SNorm,
        FET_UInt,
        FET_SInt
    };

    /// @brief Describes the element storage distribution.
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

    /// @brief Describes a graphics format.
    struct GraphicsFormat
    {
        GraphicsFormat(){}
        GraphicsFormat(GraphicsFormatElements elements, GraphicsFormatElementType elementType, GraphicsFormatElementLayout elementLayout) : elements(elements), elementType(elementType), elementLayout(elementLayout) {}

        GraphicsFormatElements elements = FE_Unknown;
        GraphicsFormatElementType elementType = FET_Unknown;
        GraphicsFormatElementLayout elementLayout = FEL_Unknown;
        bool linear = true;

        /**
         * @brief Get the amount of elements/channels in this format.
         * @return uint8 Amount of elements
         */
        uint8 GetElementCount();

        /**
         * @brief Get the size of 1 format instance.
         * @return uint8 Size of this format
         */
        uint8 GetStride();

        static GraphicsFormat Byte;
        static GraphicsFormat Byte2;
        static GraphicsFormat Byte4;

        static GraphicsFormat Float;
        static GraphicsFormat Float2;
        static GraphicsFormat Float3;
        static GraphicsFormat Float4;
    };

    extern bool operator==(const GraphicsFormat& a, const GraphicsFormat& b);

    extern bool operator<(const GraphicsFormat& a, const GraphicsFormat& b);
}
