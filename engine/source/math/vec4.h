#pragma once
#include "Vec3.h"
class Vec4
{
public:
	float e[4];
    Vec4() {}
    Vec4(float e0, float e1, float e2, float e3) { e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3; }
    Vec4(const Vec3& v3, float _e) { e[0] = v3.e[0];   e[1] = v3.e[0];  e[2] = v3.e[2]; e[3] = _e; }
    void operator=(const float* const row) { e[0] = row[0]; e[1] = row[1]; e[2] = row[2]; e[3] = row[3]; }
    //Vec4(const Vec4& v) { e[0] = v.e[0]; e[1] = v.e[1]; e[2] = v.e[2]; e[3] = v.e[3]; }
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float w() const { return e[3]; }

    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }
    inline float a() const { return e[3]; }
};