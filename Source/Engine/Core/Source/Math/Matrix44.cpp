#include "Matrix44.hpp"
#include <cmath>

namespace Faro
{
    Matrix44::Matrix44(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }
    Matrix44::Matrix44() {
        m[0][0] = 1;
        m[0][1] = 0;
        m[0][2] = 0;
        m[0][3] = 0;
        m[1][0] = 0;
        m[1][1] = 1;
        m[1][2] = 0;
        m[1][3] = 0;
        m[2][0] = 0;
        m[2][1] = 0;
        m[2][2] = 1;
        m[2][3] = 0;
        m[3][0] = 0;
        m[3][1] = 0;
        m[3][2] = 0;
        m[3][3] = 1;
    }

    Float2D Matrix44::operator*(Float2D vec) {
        Float2D out;
        out.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * 1 + m[3][0] * 1;
        out.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * 1 + m[3][1] * 1;
        return out;
    }

    Float3D Matrix44::operator*(Float3D vec) {
        Float3D out;
        out.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z + m[3][0] * 1;
        out.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z + m[3][1] * 1;
        out.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z + m[3][2] * 1;
        return out;
    }

    Float4D Matrix44::operator*(Float4D vec) {
        Float4D out;
        out.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z + m[3][0] * vec.w;
        out.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z + m[3][1] * vec.w;
        out.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z + m[3][2] * vec.w;
        out.w = m[0][3] * vec.x + m[1][3] * vec.y + m[2][3] * vec.z + m[3][3] * vec.w;
        return out;
    }

    Matrix44 Matrix44::operator+(const Matrix44& mat) const {
        Matrix44 out;
        out.f[0] = f[0] + mat.f[0];
        out.f[1] = f[1] + mat.f[1];
        out.f[2] = f[2] + mat.f[2];
        out.f[3] = f[3] + mat.f[3];
        out.f[4] = f[4] + mat.f[4];
        out.f[5] = f[5] + mat.f[5];
        out.f[6] = f[6] + mat.f[6];
        out.f[7] = f[7] + mat.f[7];
        out.f[8] = f[8] + mat.f[8];
        out.f[9] = f[9] + mat.f[9];
        out.f[10] = f[10] + mat.f[10];
        out.f[11] = f[11] + mat.f[11];
        out.f[12] = f[12] + mat.f[12];
        out.f[13] = f[13] + mat.f[13];
        out.f[14] = f[14] + mat.f[14];
        out.f[15] = f[15] + mat.f[15];
        return out;
    }
    Matrix44 Matrix44::operator-(const Matrix44& mat) const {
        Matrix44 out;
        out.f[0] = f[0] - mat.f[0];
        out.f[1] = f[1] - mat.f[1];
        out.f[2] = f[2] - mat.f[2];
        out.f[3] = f[3] - mat.f[3];
        out.f[4] = f[4] - mat.f[4];
        out.f[5] = f[5] - mat.f[5];
        out.f[6] = f[6] - mat.f[6];
        out.f[7] = f[7] - mat.f[7];
        out.f[8] = f[8] - mat.f[8];
        out.f[9] = f[9] - mat.f[9];
        out.f[10] = f[10] - mat.f[10];
        out.f[11] = f[11] - mat.f[11];
        out.f[12] = f[12] - mat.f[12];
        out.f[13] = f[13] - mat.f[13];
        out.f[14] = f[14] - mat.f[14];
        out.f[15] = f[15] - mat.f[15];
        return out;
    }
    Matrix44 Matrix44::operator*(const Matrix44& mat) const {
        //TODO speedup matrix * operator
        Matrix44 out;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                {
                    out.m[j][i] = m[j][0] * mat.m[0][i] + m[j][1] * mat.m[1][i] + m[j][2] * mat.m[2][i] + m[j][3] * mat.m[3][i];
                }
            }
        return out;
    }

    Float3D Matrix44::GetTranslation() const {
        Float3D out;
        out.x = m[3][0];
        out.y = m[3][1];
        out.z = m[3][2];
        return out;
    }
    void Matrix44::SetTranslation(const Float3D& vec) {
        m[3][0] = vec.x;
        m[3][1] = vec.y;
        m[3][2] = vec.z;
    }

    Float3D Matrix44::GetXAxis() const {
        Float3D out;
        out.x = m[0][0];
        out.y = m[0][1];
        out.z = m[0][2];
        return out;
    }
    Float3D Matrix44::GetYAxis() const {
        Float3D out;
        out.x = m[1][0];
        out.y = m[1][1];
        out.z = m[1][2];
        return out;
    }
    Float3D Matrix44::GetZAxis() const {
        Float3D out;
        out.x = m[2][0];
        out.y = m[2][1];
        out.z = m[2][2];
        return out;
    }

    float Determinant33(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33) {
        return m11 * m22*m33 + m12 * m23*m31 + m13 * m21*m32 - m13 * m22*m31 - m12 * m21*m33 - m11 * m23*m32;
    }

    float Matrix44::Determinant() const {
        return m[0][0] * Determinant33(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]) -
            m[0][1] * Determinant33(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]) +
            m[0][2] * Determinant33(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]) -
            m[0][3] * Determinant33(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
    }

    bool Matrix44::Invert() {
        float det = Determinant();
        if (det == 0) { return false; }

        Matrix44 adj;

        adj.m[0][0] = Determinant33(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
        adj.m[0][1] = Determinant33(m[2][1], m[2][2], m[2][3], m[0][1], m[0][2], m[0][3], m[3][1], m[3][2], m[3][3]);
        adj.m[0][2] = Determinant33(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
        adj.m[0][3] = Determinant33(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[1][1], m[1][2], m[1][3]);

        adj.m[1][0] = -Determinant33(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
        adj.m[1][1] = -Determinant33(m[2][0], m[2][2], m[2][3], m[0][0], m[0][2], m[0][3], m[3][0], m[3][2], m[3][3]);
        adj.m[1][2] = -Determinant33(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
        adj.m[1][3] = -Determinant33(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[1][0], m[1][2], m[1][3]);

        adj.m[2][0] = Determinant33(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
        adj.m[2][1] = Determinant33(m[2][0], m[2][1], m[2][3], m[0][0], m[0][1], m[0][3], m[3][0], m[3][1], m[3][3]);
        adj.m[2][2] = Determinant33(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
        adj.m[2][3] = Determinant33(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[1][0], m[1][1], m[1][3]);

        adj.m[3][0] = -Determinant33(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
        adj.m[3][1] = -Determinant33(m[2][0], m[2][1], m[2][2], m[0][0], m[0][1], m[0][2], m[3][0], m[3][1], m[3][2]);
        adj.m[3][2] = -Determinant33(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);
        adj.m[3][3] = -Determinant33(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[1][0], m[1][1], m[1][2]);

        for (int i = 0; i < 16; i++) {
            f[i] = adj.f[i] / det;
        }

        return true;
    }
    void Matrix44::Transpose() {
        Matrix44 self;
        for (int i = 0; i < 16; i++) { self.f[i] = f[i]; }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = self.m[j][i];
            }
        }
    }

    Matrix44 Matrix44::Transposed()
    {
        Matrix44 res;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                res.m[i][j] = m[j][i];
            }
        }

        return res;
    }

    Matrix44 Matrix44::Get()
    {
#if FARO_DX11 //TODO Replace preprocessor transposing
        return Transposed();
#endif
#if FARO_GL
        return *this;
#endif
#if FARO_GLES
        return *this;
#endif
#if FARO_WEBGL
        return Transposed();
#endif

        return Matrix44();
    }

    void Matrix44::SetOrientation(const Float3D& x, const Float3D& y, const Float3D& z) {
        m[0][0] = x.x;
        m[0][1] = x.y;
        m[0][2] = x.z;

        m[1][0] = y.x;
        m[1][1] = y.y;
        m[1][2] = y.z;

        m[2][0] = z.x;
        m[2][1] = z.y;
        m[2][2] = z.z;
    }
    void Matrix44::SetEulerAxis(float yaw, float pitch, float roll) {
        m[0][0] = cos(pitch)*cos(yaw);
        m[0][1] = cos(pitch)*sin(yaw);
        m[0][2] = -sin(pitch);
        m[1][0] = sin(roll)*sin(pitch)*cos(yaw) - cos(roll)*sin(yaw);
        m[1][1] = sin(roll)*sin(pitch)*sin(yaw) + cos(roll)*cos(yaw);
        m[1][2] = cos(pitch)*sin(roll);
        m[2][0] = cos(roll)*sin(pitch)*cos(yaw) + sin(roll)*sin(yaw);
        m[2][1] = cos(roll)*sin(pitch)*sin(yaw) - sin(roll)*cos(yaw);
        m[2][2] = cos(pitch)*cos(roll);
    }



    Matrix44 Matrix44::CreateIdentity() {
        Matrix44 out;
        return out;
    }

    Matrix44 Matrix44::CreateTranslation(Float3D translation) {
        Matrix44 out;
        out.m[3][0] = translation.x;
        out.m[3][1] = translation.y;
        out.m[3][2] = translation.z;
        return out;
    }

    Matrix44 Matrix44::CreateScale(Float3D scale) {
        Matrix44 out;
        out.m[0][0] = scale.x;
        out.m[1][1] = scale.y;
        out.m[2][2] = scale.z;
        return out;
    }

    Matrix44 Matrix44::CreateRotate(float angle, const Float3D& axis) {
        Matrix44 out;
        out.m[0][0] = powf(axis.x, 2)*(1 - cos(angle)) + cos(angle);
        out.m[0][1] = axis.x*axis.y*(1 - cos(angle)) + axis.z*sin(angle);
        out.m[0][2] = axis.x*axis.z*(1 - cos(angle)) - axis.y*sin(angle);

        out.m[1][0] = axis.x*axis.y*(1 - cos(angle)) - axis.z*sin(angle);
        out.m[1][1] = powf(axis.y, 2)*(1 - cos(angle)) + cos(angle);
        out.m[1][2] = axis.y*axis.z*(1 - cos(angle)) + axis.x*sin(angle);

        out.m[2][0] = axis.x*axis.z*(1 - cos(angle)) + axis.y*sin(angle);
        out.m[2][1] = axis.y*axis.z*(1 - cos(angle)) - axis.x*sin(angle);
        out.m[2][2] = powf(axis.z, 2)*(1 - cos(angle)) + cos(angle);

        return out;
    }

    Matrix44 Matrix44::CreateRotateX(float angle) {
        Matrix44 out;
        out.m[1][1] = cos(angle);
        out.m[2][1] = -sin(angle);
        out.m[1][2] = sin(angle);
        out.m[2][2] = cos(angle);
        return out;
    }
    Matrix44 Matrix44::CreateRotateY(float angle) {
        Matrix44 out;
        out.m[0][0] = cos(angle);
        out.m[2][0] = sin(angle);
        out.m[0][2] = -sin(angle);
        out.m[2][2] = cos(angle);
        return out;
    }
    Matrix44 Matrix44::CreateRotateZ(float angle) {
        Matrix44 out;
        out.m[0][0] = cos(angle);
        out.m[1][0] = -sin(angle);
        out.m[0][1] = sin(angle);
        out.m[1][1] = cos(angle);
        return out;;
    }

    Matrix44 Matrix44::CreateOrtho(float left, float right, float bottom, float top, float nearZ, float farZ) {
        Matrix44 out;
        out.m[0][0] = 2 / (right - left);
        out.m[3][0] = -(right + left) / (right - left);
        out.m[1][1] = 2 / (top - bottom);
        out.m[3][1] = -(top + bottom) / (top - bottom);
        out.m[2][2] = -2 / (farZ - nearZ);
        out.m[3][2] = -(farZ + nearZ) / (farZ - nearZ);
        return out;
    }

    Matrix44 Matrix44::CreateFrustum(float left, float right, float bottom, float top, float nearZ, float farZ) {
        Matrix44 out;
        out.m[0][0] = (2 * nearZ) / (right - left);
        out.m[1][1] = (2 * nearZ) / (top - bottom);
        out.m[2][0] = (right + left) / (right - left);
        out.m[2][1] = (top + bottom) / (top - bottom);
        out.m[2][1] = (top + bottom) / (top - bottom);
        out.m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        out.m[3][2] = -(2 * farZ*nearZ) / (farZ - nearZ);
        out.m[2][3] = -1;
        out.m[3][3] = 0;

        return out;
    }

    Matrix44 Matrix44::CreatePerspective(float fovy, float aspect, float nearZ, float farZ) {
        Matrix44 out;

        float fy = 1 / tanf(fovy / 2);
        out.m[0][0] = fy / aspect;
        out.m[1][1] = fy;
        out.m[2][2] = (farZ + nearZ) / (nearZ - farZ);
        out.m[3][2] = (2 * farZ * nearZ) / (nearZ - farZ);
        out.m[2][3] = -1;
        out.m[3][3] = 0;
        return out;
    }

    Matrix44 Matrix44::CreateLookAt(const Float3D& eye, const Float3D& center, const Float3D& up) {
        Matrix44 out;

        Float3D z = (eye - center); z.Normalize();
        Float3D x = up.Cross(z); x.Normalize();
        Float3D y = z.Cross(x);


        out.m[0][0] = x.x;
        out.m[1][0] = x.y;
        out.m[2][0] = x.z;

        out.m[0][1] = y.x;
        out.m[1][1] = y.y;
        out.m[2][1] = y.z;

        out.m[0][2] = z.x;
        out.m[1][2] = z.y;
        out.m[2][2] = z.z;

        out.m[3][0] = -x.Dot(eye);
        out.m[3][1] = -y.Dot(eye);
        out.m[3][2] = -z.Dot(eye);

        return out;
    }

    Float3D Matrix44::TransformDirectionVector(const Float3D& direction) {
        Float3D out;
        out.x = m[0][0] * direction.x + m[1][0] * direction.y + m[2][0] * direction.z;
        out.y = m[0][1] * direction.x + m[1][1] * direction.y + m[2][1] * direction.z;
        out.z = m[0][2] * direction.x + m[1][2] * direction.y + m[2][2] * direction.z;
        return out;
    }
}
