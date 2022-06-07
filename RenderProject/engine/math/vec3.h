#pragma once
#include <math.h>
#include <stdlib.h>
#include "Matr.h"


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

    inline Vec3 normalized() { return *this / length(); }
    inline Vec3& normalize() { return *this /= length(); }

    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

    inline static float dot(const Vec3 &v1, const Vec3& v2);
    inline static Vec3 cross(const Vec3 &v1, const Vec3& v2);
    inline static float length(const Vec3& v);
    inline static Vec3 lerp(const Vec3& a, const Vec3& b, float t);

    inline Vec3& mult(const Matr& matr, float w);

    Vec3(const std::vector<float>& vec)
    {
        e[0] = vec[0]; e[1] = vec[1]; e[2] = vec[2];
    }

    float e[3];
};

inline float Vec3::length(const Vec3& v)
{
    return sqrt(v.e[0] * v.e[0] + v.e[1] * v.e[1] + v.e[2] * v.e[2]);
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

inline Vec3& Vec3::mult(const Matr& matr, float w)
{
    Vec3 vec;
    float sum = 0;
    if (matr.size() == 4)
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
        {
            sum += e[j] * matr[j][i];
        }
        sum += w * matr[3][i];
        vec[i] = sum;
    }
    else if(matr.size() == 3)
    for (int i = 0; i < 3; i++)
    {
        sum = 0;
        for (int j = 0; j < 3; j++)
        {
            sum += e[j] * matr[j][i];
        }
        vec[i] = sum;
    }
    for (int i = 0; i < 3; i++)
    {
        e[i] = vec[i];
    }
    return *this;
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