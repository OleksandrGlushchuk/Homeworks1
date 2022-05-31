#pragma once
#include "vec3.h"
#include "ray.h"
#include "Intersection.h"
namespace math
{
	class Triangle
	{
	public:
		Vec3 v0, v1, v2;
		Vec3 normal;
		Triangle(const Vec3& _v0, const Vec3& _v1, const Vec3& _v2, const Vec3& _normal) :v0(_v0), v1(_v1), v2(_v2), normal(_normal) {}
		bool intersection(math::Intersection& nearest, const ray& r) const;
	};
}