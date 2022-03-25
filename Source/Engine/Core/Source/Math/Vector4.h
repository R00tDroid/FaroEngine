#pragma once
#include "../Containers/String.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Faro
{
	namespace Math
	{

		template<class T>
		struct Vector4
		{
			union
			{
				T f[4];
#pragma warning(suppress: 4201)
				struct { T x, y, z, w; };
			};

			Vector4() 
			{
				Clear();
			}
			Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
			Vector4(Vector3<T> v, T w) : x(v.x), y(v.y), z(v.z), w(w) {}
			Vector4(Vector2<T> a, Vector2<T> b) : x(a.x), y(a.y), z(b.x), w(b.y) {}

			Vector3<T> xyz()
			{
				return { x, y, z };
			}

			Vector2<T> xy()
			{
				return { x, y };
			}

			Vector2<T> zw()
			{
				return { z, w };
			}

			Vector2<T> yz()
			{
				return { y, z };
			}

			void Clear()
			{
				x = 0; y = 0; z = 0; w = 0;
			}

			Vector4 Lerp(const Vector4& r, float fact) const { return Vector4(x + ((r.x - x)*fact), y + ((r.y - y)*fact), z + ((r.z - z)*fact), z + ((r.w - w)*fact)); }

			String ToString()
			{
				return "(x: " + Faro::ToString<T>(x) + ", y: " + Faro::ToString<T>(y) + ", z: " + Faro::ToString<T>(z) + ", w: " + Faro::ToString<T>(w) + ")";
			}

			template<class N> Vector4<N> ToType()
			{
				return Vector4<N>((N)x, (N)y, (N)z, (N)w);
			}
		};

		typedef Vector4<float> Float4D;
		typedef Vector4<int32> Int4D;
	}
}
