#ifndef Faro_MATHVEC3
#define Faro_MATHVEC3
#include "Vector2.h"
#include "../Containers/String.h"

namespace Faro
{
    template<class T>
    struct Vector3
    {
        union
        {
            T f[3];
            #pragma warning(suppress: 4201)
            struct { T x, y, z; };
        };

        Vector3() { Clear(); }

        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

        Vector3(Vector2<T> v, T z) : x(v.x), y(v.y), z(z) {}

        Vector2<T> xy()
        {
            return { x, y };
        }

        Vector2<T> yz()
        {
            return { y, z };
        }

        Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }

        Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }

        Vector3 operator*(const float value) const { return Vector3(x * value, y * value, z * value); }

        Vector3 operator/(const float value) const { return Vector3(x / value, y / value, z / value); }

        Vector3 operator-() const { return Vector3(-x, -y, -z); }

        void operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; }

        void operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; }

        void operator*=(const float value) { x *= value; y *= value; z *= value; }

        Vector3 Cross(const Vector3& vector) const { return Vector3(y*vector.z - z * vector.y, z*vector.x - x * vector.z, x*vector.y - y * vector.x); }

        float Dot(const Vector3& vector) const { return x * vector.x + y * vector.y + z * vector.z; }

        T Magnitude() const { return std::sqrt(x*x + y * y + z * z); }

        T SquareMagnitude() const { return x * x + y * y + z * z; }

        void Normalize() { T l = Magnitude(); x /= l; y /= l; z /= l; }

        Vector3 Normalized() { T l = Magnitude(); return Vector3(x /= l, y /= l, z /= l); }

        bool operator==(const Vector3& other) const {
            for (int i = 0; i < 3; i++) {
                if (f[i] != other.f[i]) return false;
            }
            return true;
        }

        bool operator!=(const Vector3& other) const {
            for (int i = 0; i < 3; i++) {
                if (f[i] != other.f[i]) return true;
            }
            return false;
        }

        void Clear() { x = 0; y = 0; z = 0; }

        Vector3 Lerp(const Vector3& r, float fact) const { return Vector3(x + ((r.x - x)*fact), y + ((r.y - y)*fact), z + ((r.z - z)*fact)); }

        static Vector3 Up;
        static Vector3 Down;
        static Vector3 Forward;
        static Vector3 Back;
        static Vector3 Left;
        static Vector3 Right;

        String ToString() 
        {
            return "(x: " + Faro::ToString<T>(x) + ", y: " + Faro::ToString<T>(y) + ", z: " + Faro::ToString<T>(z) + ")";
        }

        template<class N> Vector3<N> ToType()
        {
            return Vector3<N>((N)x, (N)y, (N)z);
        }
    };

    typedef Vector3<float> Float3D;
    typedef Vector3<int32> Int3D;

}
#endif