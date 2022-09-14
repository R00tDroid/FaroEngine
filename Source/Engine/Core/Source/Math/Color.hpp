#pragma once
#include "../Containers/String.h"

namespace Faro
{
    template <class T, int TMax>
    struct IColor
    {
        union
        {
            T d[4];
            #pragma warning(suppress: 4201)
            struct { T r, g, b, a; };
        };

        IColor(T r, T g, T b, T a = TMax) : r(r), g(g), b(b), a(a) { }
        IColor(float color[4]) : r(color[0]), g(color[1]), b(color[2]), a(color[3]) { }
        IColor() : r(0), g(0), b(0), a(0) { }

        IColor operator+ (IColor& other)
        {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }

        IColor operator+ (IColor other)
        {
            return Color(r + other.r, g + other.g, b + other.b, a + other.a);
        }

        IColor operator- (IColor& other)
        {
            return Color(r - other.r, g - other.g, b - other.b, a - other.a);
        }

        IColor operator- (IColor other)
        {
            return Color(r - other.r, g - other.g, b - other.b, a - other.a);
        }

        IColor Lerp(IColor& other, float time) const
        {
            return Color((1 - time) * r + other.r * time, (1 - time) * g + other.g * time, (1 - time) * b + other.b * time, (1 - time) * a + other.a * time);
        }

        // Predefined colors
        static IColor Red;
        static IColor Green;
        static IColor Blue;
        static IColor White;
        static IColor Black;
        static IColor Clear;

#ifdef _MSC_VER //TODO use engine preprocessor defines
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif
        static IColor FromHex(String hex)
        {
            if (hex[0] == '#') hex.Erase(0);
            int r = 255, g = 255, b = 255, a = 255;

            if (hex.Length() == 6)
            {
                SSCANF(*hex, "%02x%02x%02x", &r, &g, &b);
            }
            else if (hex.Length() == 8)
            {
                SSCANF(*hex, "%02x%02x%02x%02x", &r, &g, &b, &a);
            }

            return IColor((r / 255.0f) * static_cast<float>(TMax), (g / 255.0f) * static_cast<float>(TMax), (b / 255.0f) * static_cast<float>(TMax), (a / 255.0f) * static_cast<float>(TMax));
        }
    };

    typedef IColor<float, 1> FloatColor;
    typedef IColor<uint8, 255> UInt8Color;

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::Red = IColor<T, TMax>(TMax, 0, 0, TMax);

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::Green = IColor<T, TMax>(0, TMax, 0, TMax);

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::Blue = IColor<T, TMax>(0, 0, TMax, TMax);

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::White = IColor<T, TMax>(TMax, TMax, TMax, TMax);

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::Black = IColor<T, TMax>(0, 0, 0, TMax);

    template <class T, int TMax>
    IColor<T, TMax> IColor<T, TMax>::Clear = IColor<T, TMax>(0, 0, 0, 0);
}
