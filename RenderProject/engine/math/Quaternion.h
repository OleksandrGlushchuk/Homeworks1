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
	Quaternion(){}
	Quaternion(float angle,const Vec3 &_axis)
	{
		w = cos(angle/2.f);
		float sin_a_2 = sin(angle / 2.f);
		vec.e[0] = sin_a_2 * _axis.e[0];
		vec.e[1] = sin_a_2 * _axis.e[1];
		vec.e[2] = sin_a_2 * _axis.e[2];
	}
	inline void operator*=(const Quaternion& b)
	{
		Quaternion a = *this;
		a.w = b.w * this->w - Vec3::dot(b.vec,this->vec);
		a.vec = this->w * b.vec + b.w * this->vec + Vec3::cross(this->vec,b.vec);
		this->w = a.w;
		this->vec = a.vec;
	}
	inline Quaternion operator*(const Quaternion& b) const
	{
		Quaternion a = *this;
		a.w = b.w * this->w - Vec3::dot(b.vec, this->vec);
		a.vec = this->w * b.vec + b.w * this->vec + Vec3::cross(this->vec, b.vec);
		return a;
	}
	inline void operator*=(const float val)
	{
		w *= val;
		vec.e[0] *= val;
		vec.e[1] *= val;
		vec.e[2] *= val;
	}
	inline void operator/=(const float val)
	{
		this->operator*=(1.f/val);
	}
	inline float norma() const
	{
		return sqrt(w * w + vec.x() * vec.x() + vec.y() * vec.y() + vec.z() * vec.z());
	}
	inline void normalize()
	{
		(*this) *= 1.f / norma();
	}
	inline Matr<3> toMat3() const
	{
		Matr<3> out;
		Matr<3>::fill_row(out[0], { 1.0f - 2.0f * vec.y() * vec.y() - 2.0f * vec.z() * vec.z(), 2.0f * vec.x() * vec.y() - 2.0f * vec.z() * w, 2.0f * vec.x() * vec.z() + 2.0f * vec.y() * w });
		Matr<3>::fill_row(out[1], { 2.0f * vec.x() * vec.y() + 2.0f * vec.z() * w, 1.0f - 2.0f * vec.x() * vec.x() - 2.0f * vec.z() * vec.z(), 2.0f * vec.y() * vec.z() - 2.0f * vec.x() * w });
		Matr<3>::fill_row(out[2], { 2.0f * vec.x() * vec.z() - 2.0f * vec.y() * w, 2.0f * vec.y() * vec.z() + 2.0f * vec.x() * w, 1.0f - 2.0f * vec.x() * vec.x() - 2.0f * vec.y() * vec.y() });
		return out;
	}
	inline Quaternion invert() const
	{
		Quaternion res;
		res.w = w;
		res.vec = -vec;
		float norm = norma();
		res /= (norm*norm);
		return res;
	}
	inline static Quaternion identity();
};

inline Quaternion Quaternion::identity()
{
	Quaternion res;
	res.w = 1;
	res.vec = Vec3(0, 0, 0);
	return res;
}