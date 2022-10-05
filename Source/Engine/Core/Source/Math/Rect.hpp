#pragma once
#include "../Containers/String.hpp"
#include "Vector2.hpp"

namespace Faro
{
    template <class T>
    struct IRect
    {
        union
        {
            Vector2<T> points[2];
#pragma warning(suppress: 4201)
            struct { Vector2<T> topLeft, bottomRight; };
#pragma warning(suppress: 4201)
            struct { T x1, y1, x2, y2; };
        };

        IRect() : x1(0), y1(0), x2(0), y2(0) { }

        IRect(T x, T y, T w, T h) : x1(x), y1(y), x2(x + w), y2(y + h) {}

        IRect(Vector2<T> topLeft, Vector2<T> extend) : topLeft(topLeft), bottomRight(topLeft + extend) {}

        T Area() const { Vector2<T> size = Size(); return size.x * size.y; }

        Vector2<T> Size() const { return bottomRight - topLeft; }

        Vector2<T> Center()
        {
            return topLeft + Size() / 2;
        }

        bool operator==(const IRect& other) const
        {
            return topLeft == other.topLeft && bottomRight == other.bottomRight;
        }

        bool operator!=(const Vector2<T>& other) const 
        {
            return topLeft != other.topLeft || bottomRight != other.bottomRight;
        }

        IRect Lerp(const IRect& r, float fact) const { return IRect(topLeft.Lerp(r.topLeft, fact), Size().Lerp(r.Size()), fact); }

        String ToString()
        {
            return "(x: " + Faro::ToString<T>(x1) + ", y: " + Faro::ToString<T>(y1) + ", w: " + Faro::ToString<T>(x2 - x1) + ", h: " + Faro::ToString<T>(y2 - y1) + ")";
        }

        template<class N>
        IRect<N> ToType()
        {
            return IRect<N>((N)x1, (N)y1, (N)(x2 - x1), (N)(y2 - y1));
        }
    };

    typedef IRect<float> FloatRect;
    typedef IRect<int32> IntRect;
}
