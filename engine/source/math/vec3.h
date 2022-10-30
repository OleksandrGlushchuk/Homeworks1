#pragma once
#include <math.h>
#include <cstdint>
#include <stdlib.h>

template<uint32_t SIZE, typename TYPE>
class Matr;

class Vec3 {
public:
    Vec3() {}
    Vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const Vec3& operator+() const { return *this; }
    inline Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    inline Vec3& operator+=(const Vec3& v2);
    inline Vec3& operator-=(const Vec3& v2);
    inline Vec3& operator*=(const Vec3& v2);
    inline Vec3& operator/=(const Vec3& v2);

    inline Vec3& operator*=(const float t);
    inline Vec3& operator/=(const float t);

    inline Vec3 operator+(const Vec3& v) const { return Vec3(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);}
    inline Vec3 operator-(const Vec3& v) const { return Vec3(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]);}
    inline Vec3 operator*(const Vec3& v) const { return Vec3(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]); }
    inline Vec3 operator/(const Vec3& v) const { return Vec3(e[0] / v.e[0], e[1] / v.e[1], e[2] / v.e[2]); }


    inline Vec3 operator*(float t) const { return Vec3(t * e[0], t * e[1], t * e[2]);}
    inline Vec3 operator/(float t) const {return Vec3(e[0] / t, e[1] / t, e[2] / t);}

    inline Vec3 normalized() const { return *this / length(); }
    inline Vec3& normalize() { return *this /= length(); }

    inline float length() const { return sqrtf(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

    inline static float dot(const Vec3 &v1, const Vec3& v2);
    inline static Vec3 cross(const Vec3 &v1, const Vec3& v2);
    inline static float length(const Vec3& v);
    inline static Vec3 lerp(const Vec3& a, const Vec3& b, float t);

    inline static Vec3 Reflect(const Vec3& vec, const Vec3& normal);

    Vec3& mult(const Matr<4, float>& matr, float w = 1);
    Vec3& mult(const Matr<4, float>& matr, float w, float *w_out);
    Vec3& mult(const Matr<3, float>& matr);

    inline static void GetFrisvadsBasis(const Vec3& n, Vec3& b1, Vec3& b2);
    Vec3(const float*const & vec)
    {
        e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
    }

    Vec3 operator=(const float* const& vec)
    {
        e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
        return *this;
    }

    bool operator==(const Vec3& vec) const
    {
        return e[0] == vec.e[0] && e[1] == vec.e[1] && e[2] == vec.e[2];
    }
    bool operator!=(const Vec3& vec) const
    {
        return e[0] != vec.e[0] && e[1] != vec.e[1] && e[2] != vec.e[2];
    }
    
    float e[3];
};

inline float Vec3::length(const Vec3& v)
{
    return sqrtf(v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2]);
}

inline Vec3 Vec3::lerp(const Vec3& a, const Vec3& b, float t)
{
    return a + (b - a) * t;
}

inline float Vec3::dot(const Vec3& v1, const Vec3& v2)
{
    return v1.e[0] * v2.e[0]
        + v1.e[1] * v2.e[1]
        + v1.e[2] * v2.e[2];
}

inline Vec3 Vec3::cross(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
        v1.e[2] * v2.e[0] - v1.e[0] * v2.e[2],
        v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

inline Vec3 operator*(float t, const Vec3& v) { return v * t; }
inline Vec3 operator-(float t, const Vec3& v) { return Vec3(t-v.e[0],t-v.e[1],t-v.e[2]); }

inline Vec3& Vec3::operator+=(const Vec3& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

inline Vec3& Vec3::operator*=(const Vec3& v) {
    e[0] *= v.e[0];
    e[1] *= v.e[1];
    e[2] *= v.e[2];
    return *this;
}

inline Vec3& Vec3::operator/=(const Vec3& v) {
    e[0] /= v.e[0];
    e[1] /= v.e[1];
    e[2] /= v.e[2];
    return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

inline Vec3& Vec3::operator*=(const float t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline Vec3& Vec3::operator/=(const float t) {
    float k = 1.0f / t;

    e[0] *= k;
    e[1] *= k;
    e[2] *= k;
    return *this;
}

inline Vec3 Vec3::Reflect(const Vec3& vec, const Vec3& normal)
{
    return vec - 2.f * Vec3::dot(vec, normal) * normal;
}

inline void Vec3::GetFrisvadsBasis(const Vec3& n, Vec3& b1, Vec3& b2)
{
    float sign = n.z() >= 0.0f ? 1.0f : -1.0f;
    const float a = -1.0f / (sign + n.z());
    const float b = n.x() * n.y() * a;
    b1 = Vec3(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
    b2 = Vec3(b, sign + n.y() * n.y() * a, -n.y());
}