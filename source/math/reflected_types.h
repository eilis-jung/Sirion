#pragma once

#include "base.h"

namespace Sirion
{
    // For math structs that can be reflected, but still compatible with GLM
    // To convert a reflection type to a base math type, use `toBase()`.
    // Unfortunately not sure how to write template with reflection constructor yet...
    REFLECTION_TYPE(ReflectedVector3)
    CLASS(ReflectedVector3, Fields)
    {
        REFLECTION_BODY(ReflectedVector3);

    public:
        ReflectedVector3() = default;
        ReflectedVector3(float x_, float y_, float z_) : x {x_}, y {y_}, z {z_} {}
        ReflectedVector3(Vector3 & v) : x(v.x), y(v.y), z(v.z) {}
        Vector3 toBase() { return Vector3(x, y, z); }

        float operator[](size_t i) const
        {
            assert(i < 3);
            return *(&x + i);
        }

        float& operator[](size_t i)
        {
            assert(i < 3);
            return *(&x + i);
        }
        /// Pointer accessor for direct copying
        float* ptr() { return &x; }
        /// Pointer accessor for direct copying
        const float* ptr() const { return &x; }

        bool operator==(const ReflectedVector3& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }

        bool operator!=(const ReflectedVector3& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

        // arithmetic operations
        ReflectedVector3 operator+(const ReflectedVector3& rhs) const
        {
            return ReflectedVector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        ReflectedVector3 operator-(const Vector3& rhs) const
        {
            return ReflectedVector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        ReflectedVector3 operator*(float scalar) const { return ReflectedVector3(x * scalar, y * scalar, z * scalar); }

        ReflectedVector3 operator*(const ReflectedVector3& rhs) const
        {
            return ReflectedVector3(x * rhs.x, y * rhs.y, z * rhs.z);
        }

        ReflectedVector3 operator/(float scalar) const
        {
            assert(scalar != 0.0);
            return ReflectedVector3(x / scalar, y / scalar, z / scalar);
        }

        ReflectedVector3 operator/(const ReflectedVector3& rhs) const
        {
            assert((rhs.x != 0 && rhs.y != 0 && rhs.z != 0));
            return ReflectedVector3(x / rhs.x, y / rhs.y, z / rhs.z);
        }

        const ReflectedVector3& operator+() const { return *this; }

        ReflectedVector3 operator-() const { return ReflectedVector3(-x, -y, -z); }

        // overloaded operators to help Vector3
        friend ReflectedVector3 operator*(float scalar, const ReflectedVector3& rhs)
        {
            return ReflectedVector3(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
        }

        friend ReflectedVector3 operator/(float scalar, const ReflectedVector3& rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            return ReflectedVector3(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
        }

        friend ReflectedVector3 operator+(const ReflectedVector3& lhs, float rhs)
        {
            return ReflectedVector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
        }

        friend ReflectedVector3 operator+(float lhs, const ReflectedVector3& rhs)
        {
            return ReflectedVector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
        }

        friend ReflectedVector3 operator-(const ReflectedVector3& lhs, float rhs)
        {
            return ReflectedVector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
        }

        friend ReflectedVector3 operator-(float lhs, const ReflectedVector3& rhs)
        {
            return ReflectedVector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
        }

        // arithmetic updates
        ReflectedVector3& operator+=(const ReflectedVector3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        ReflectedVector3& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            return *this;
        }

        ReflectedVector3& operator-=(const ReflectedVector3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        ReflectedVector3& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            return *this;
        }

        ReflectedVector3& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        ReflectedVector3& operator*=(const ReflectedVector3& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            return *this;
        }

        ReflectedVector3& operator/=(float scalar)
        {
            assert(scalar != 0.0);
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
        }

        ReflectedVector3& operator/=(const ReflectedVector3& rhs)
        {
            assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            return *this;
        }

    private:
        float x {0.f};
        float y {0.f};
        float z {0.f};
    };

    REFLECTION_TYPE(ReflectedVector4)
    CLASS(ReflectedVector4, Fields)
    {
        REFLECTION_BODY(ReflectedVector4);

    public:
        ReflectedVector4() = default;
        ReflectedVector4(float x_, float y_, float z_, float w_) : x {x_}, y {y_}, z {z_}, w {w_} {}
        ReflectedVector4(ReflectedVector4 & v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        ReflectedVector4 toBase() { return ReflectedVector4(x, y, z, w); }
        operator Vector4() const
        {
            return ReflectedVector4(x, y, z, w);
            ;
        }

        //explicit ReflectedVector4(float coords[4]) : x {coords[0]}, y {coords[1]}, z {coords[2]}, w {coords[3]} {}

        //float operator[](size_t i) const
        //{
        //    assert(i < 4);
        //    return *(&x + i);
        //}

        //float& operator[](size_t i)
        //{
        //    assert(i < 4);
        //    return *(&x + i);
        //}

        ///// Pointer accessor for direct copying
        //float* ptr() { return &x; }
        ///// Pointer accessor for direct copying
        //const float* ptr() const { return &x; }

        //ReflectedVector4& operator=(float scalar)
        //{
        //    x = scalar;
        //    y = scalar;
        //    z = scalar;
        //    w = scalar;
        //    return *this;
        //}

        bool operator==(const ReflectedVector4& rhs) const
        {
            return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
        }

        //bool operator!=(const ReflectedVector4& rhs) const { return !(rhs == *this); }

        //ReflectedVector4 operator+(const ReflectedVector4& rhs) const
        //{
        //    return ReflectedVector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        //}
        //ReflectedVector4 operator-(const ReflectedVector4& rhs) const
        //{
        //    return ReflectedVector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        //}
        //ReflectedVector4 operator*(float scalar) const
        //{
        //    return ReflectedVector4(x * scalar, y * scalar, z * scalar, w * scalar);
        //}
        //ReflectedVector4 operator*(const ReflectedVector4& rhs) const
        //{
        //    return ReflectedVector4(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w);
        //}
        //ReflectedVector4 operator/(float scalar) const
        //{
        //    assert(scalar != 0.0);
        //    return ReflectedVector4(x / scalar, y / scalar, z / scalar, w / scalar);
        //}
        //ReflectedVector4 operator/(const ReflectedVector4& rhs) const
        //{
        //    assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        //    return ReflectedVector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
        //}

        //const ReflectedVector4& operator+() const { return *this; }

        //ReflectedVector4 operator-() const { return ReflectedVector4(-x, -y, -z, -w); }

        //friend ReflectedVector4 operator*(float scalar, const ReflectedVector4& rhs)
        //{
        //    return ReflectedVector4(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
        //}

        //friend ReflectedVector4 operator/(float scalar, const ReflectedVector4& rhs)
        //{
        //    assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        //    return ReflectedVector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
        //}

        //friend ReflectedVector4 operator+(const ReflectedVector4& lhs, float rhs)
        //{
        //    return ReflectedVector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
        //}

        //friend ReflectedVector4 operator+(float lhs, const ReflectedVector4& rhs)
        //{
        //    return ReflectedVector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
        //}

        //friend ReflectedVector4 operator-(const ReflectedVector4& lhs, float rhs)
        //{
        //    return ReflectedVector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
        //}

        //friend ReflectedVector4 operator-(float lhs, const ReflectedVector4& rhs)
        //{
        //    return ReflectedVector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
        //}

        //// arithmetic updates
        //ReflectedVector4& operator+=(const ReflectedVector4& rhs)
        //{
        //    x += rhs.x;
        //    y += rhs.y;
        //    z += rhs.z;
        //    w += rhs.w;
        //    return *this;
        //}

        //ReflectedVector4& operator-=(const ReflectedVector4& rhs)
        //{
        //    x -= rhs.x;
        //    y -= rhs.y;
        //    z -= rhs.z;
        //    w -= rhs.w;
        //    return *this;
        //}

        //ReflectedVector4& operator*=(float scalar)
        //{
        //    x *= scalar;
        //    y *= scalar;
        //    z *= scalar;
        //    w *= scalar;
        //    return *this;
        //}

        //ReflectedVector4& operator+=(float scalar)
        //{
        //    x += scalar;
        //    y += scalar;
        //    z += scalar;
        //    w += scalar;
        //    return *this;
        //}

        //ReflectedVector4& operator-=(float scalar)
        //{
        //    x -= scalar;
        //    y -= scalar;
        //    z -= scalar;
        //    w -= scalar;
        //    return *this;
        //}

        //ReflectedVector4& operator*=(const ReflectedVector4& rhs)
        //{
        //    x *= rhs.x;
        //    y *= rhs.y;
        //    z *= rhs.z;
        //    w *= rhs.w;
        //    return *this;
        //}

        //ReflectedVector4& operator/=(float scalar)
        //{
        //    assert(scalar != 0.0);

        //    x /= scalar;
        //    y /= scalar;
        //    z /= scalar;
        //    w /= scalar;
        //    return *this;
        //}

        //ReflectedVector4& operator/=(const ReflectedVector4& rhs)
        //{
        //    assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        //    x /= rhs.x;
        //    y /= rhs.y;
        //    z /= rhs.z;
        //    w /= rhs.w;
        //    return *this;
        //}

    private:
        float x {0.f};
        float y {0.f};
        float z {0.f};
        float w {0.f};
    };

    // REFLECTION_TYPE(ReflectedMatrix4)
    // CLASS(ReflectedMatrix4, Fields)
    //{
    //     REFLECTION_BODY(ReflectedMatrix4);

    // public:
    //     ReflectedMatrix4() = default;
    //     ReflectedMatrix4(Matrix4 & v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    //     Matrix4 toBase() { return Matrix4(x, y, z, w); }

    // private:
    //     float x {0.f};
    //     float y {0.f};
    //     float z {0.f};
    //     float w {0.f};
    // };
} // namespace Sirion