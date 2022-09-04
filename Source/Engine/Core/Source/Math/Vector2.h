#ifndef Faro_MATHVEC2
#define Faro_MATHVEC2
#include <cmath>
#include "../Containers/String.h"

namespace Faro
{
    template <class T>
    struct Vector2
    {
        union
        {
            T f[2];
#pragma warning(suppress: 4201)
            struct { T x, y; };
        };

        Vector2() { Clear(); }

        Vector2(T x, T y) : x(x), y(y) {}

        Vector2 operator+(const Vector2<T>& v) const { return Vector2(x + v.x, y + v.y); }

        Vector2 operator-(const Vector2<T>& v) const { return Vector2(x - v.x, y - v.y); }

        Vector2 operator*(const T value) const { return Vector2(x * value, y * value); }

        Vector2 operator/(const T value) const { return Vector2(x / value, y / value); }

        Vector2 operator-() const { return Vector2(-x, -y); }

        Vector2 operator *(const Vector2<T>& vector) const { return Vector2(x * vector.x, y * vector.y); }

        void operator+=(const Vector2<T>& v) { x += v.x; y += v.y; }

        void operator-=(const Vector2<T>& v) { x -= v.x; y -= v.y; }

        void operator*=(const T value) { x *= value; y *= value; }

        float Dot(const Vector2<T>& vector) const
        {
            return x * vector.x + y * vector.y;
        }

        float Cross(const Vector2<T>& vector) const
        {
            return (x * vector.y) - (y * vector.x);
        }

        static float Dot(const Vector2<T>& a, const Vector2<T>& b)
        {
            return a.x * b.x + a.y * b.y;
        }

        static float Cross(const Vector2<T>& a, const Vector2<T>& b)
        {
            return (a.x * b.y) - (a.y * b.x);
        }

        T Magnitude() const { return std::sqrt(x*x + y * y); }

        T SquareMagnitude() const { return x * x + y * y; }

        void Normalize() { T l = Magnitude(); x /= l; y /= l; }

        bool operator==(const Vector2<T>& other) const 
        {
            for (int i = 0; i < 2; i++) {
                if (f[i] != other.f[i]) return false;
            }
            return true;
        }

        bool operator!=(const Vector2<T>& other) const 
        {
            for (int i = 0; i < 2; i++) 
            {
                if (f[i] != other.f[i]) return true;
            }
            return false;
        }

        void Clear() { x = 0; y = 0; }

        Vector2<T> Lerp(const Vector2<T>& r, float fact) const { return Vector2(x + ((r.x - x) * fact), y + ((r.y - y) * fact)); }

        String ToString()
        {
            return "(x: " + Faro::ToString<T>(x) + ", y: " + Faro::ToString<T>(y) + ")";
        }

        template<class N> Vector2<N> ToType()
        {
            return Vector2<N>((N)x, (N)y);
        }
    };


    typedef Vector2<float> Float2D;
    typedef Vector2<int32> Int2D;
}

#endif