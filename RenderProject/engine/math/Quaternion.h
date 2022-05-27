#pragma once
#include "vec3.h"
#include "Matr.h"
#define _USE_MATH_DEFINES
#include <math.h>
class Quaternion
{
public:
	Vec3 vec;
	float w;
	Quaternion()
	{
		w = 1;
		vec.e[0] = vec.e[1] = vec.e[2] = 0;
	}
	Quaternion(float angle,const Vec3 &_axis)
	{
		angle = angle * M_PI / 180.f;
		w = cos(angle/2.f);
		float sin_a_2 = sin(angle / 2.f);
		vec.e[0] = sin_a_2 * _axis.e[0];
		vec.e[1] = sin_a_2 * _axis.e[1];
		vec.e[2] = sin_a_2 * _axis.e[2];
	}
	inline void operator*=(const Quaternion& b)
	{
		Quaternion a = *this;
		a.w = b.w * this->w - dot(b.vec, this->vec);
		a.vec = this->w * b.vec + b.w * this->vec + cross(this->vec,b.vec);
		this->w = a.w;
		this->vec = a.vec;
	}
	inline void operator*=(const float val)
	{
		w *= val;
		vec.e[0] *= val;
		vec.e[1] *= val;
		vec.e[2] *= val;
	}
	inline float norma()
	{
		return sqrt(w * w + vec.x() * vec.x() + vec.y() * vec.y() + vec.z() * vec.z());
	}
	inline void normalize()
	{
		(*this) *= 1.f / norma();
	}
	inline Matr toMat3() const
	{
		Matr out(3);
		out[0] = { 1.0f - 2.0f * vec.y() * vec.y() - 2.0f * vec.z() * vec.z(), 2.0f * vec.x() * vec.y() - 2.0f * vec.z() * w, 2.0f * vec.x() * vec.z() + 2.0f * vec.y() * w };
		out[1] = { 2.0f * vec.x() * vec.y() + 2.0f * vec.z() * w, 1.0f - 2.0f * vec.x() * vec.x() - 2.0f * vec.z() * vec.z(), 2.0f * vec.y() * vec.z() - 2.0f * vec.x() * w };
		out[2] = { 2.0f * vec.x() * vec.z() - 2.0f * vec.y() * w, 2.0f * vec.y() * vec.z() + 2.0f * vec.x() * w, 1.0f - 2.0f * vec.x() * vec.x() - 2.0f * vec.y() * vec.y() };
		return out;
	}
};