#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Faro
{
	namespace Math 
	{

		struct Matrix44
		{
			union
			{
				float m[4][4];
				float f[16];
			};

			Matrix44(float m00, float m01, float m02, float m03,
				float m10, float m11, float m12, float m13,
				float m20, float m21, float m22, float m23,
				float m30, float m31, float m32, float m33);

			Matrix44();

			Float2D operator*(Float2D vec);

			Float3D operator*(Float3D vec);

			Float4D operator*(Float4D vec);

			Matrix44 operator+(const Matrix44& mat) const;

			Matrix44 operator-(const Matrix44& mat) const;

			Matrix44 operator*(const Matrix44& mat) const;

			/// Translation bit of the matrix
			Float3D GetTranslation() const;

			/// Set the transltion of the matrix
			void SetTranslation(const Float3D& vec);

			/// Get the x orientation axis 
			Float3D GetXAxis() const;

			/// Get the y orientation axis 
			Float3D GetYAxis() const;

			/// Get the z orientation axis 
			Float3D GetZAxis() const;

			/// Get the determinant of this matrix
			float Determinant() const;

			/// Inverts this matrix
			bool Invert();

			/// Transposes this matrix
			void Transpose();

			/// Returns tthis matrix transposed
			Matrix44 Transposed();

			Matrix44 Get();

			/// Sets the orientation of the matrix to the orthogonal basis vector
			/// It perfoms no checks on the orthogonality!
			///
			/// @param x X orthogonal basis vector
			/// @param y Y orthogonal basis vector
			/// @param z Z orthogonal basis vector
			void SetOrientation(const Float3D& x,
				const Float3D& y,
				const Float3D& z);

			/// Set orientation using Euler angles. Broken at current!
			void SetEulerAxis(float yaw, float pitch, float roll);

			/// Creates an identity matrix
			///
			/// @return Identity matrix
			static Matrix44 CreateIdentity();

			/// Creates a transation matrix
			///
			/// @return Translation matrix
			static Matrix44 CreateTranslation(Float3D translation);

			static Matrix44 CreateScale(Float3D scale);

			/// Creates a rotation matrix around an arbitrary axis
			static Matrix44 CreateRotate(float angle, const Float3D& axis);

			/// Angle in radians
			static Matrix44 CreateRotateX(float angle);

			/// Angle in radians
			static Matrix44 CreateRotateY(float angle);

			/// Angle in radians
			static Matrix44 CreateRotateZ(float angle);

			/// Creates an orthographic projection matrix
			static Matrix44 CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ);

			/// Creates a frustum projection matrix
			static Matrix44 CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ);

			/// Creates a perspective projection matrix from camera settings
			static Matrix44 CreatePerspective(float fovy, float aspect, float nearZ, float farZ);

			/// Creates a look at matrix, usualy a view matrix  
			static Matrix44 CreateLookAt(const Float3D& eye, const Float3D& center, const Float3D& up);

			/// Transfrom just the direction
			Float3D TransformDirectionVector(const Float3D& direction);
		};

	}
}