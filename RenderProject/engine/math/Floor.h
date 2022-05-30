#pragma once
#include "ray.h"
#include "Intersection.h"
namespace math
{
	class Floor
	{
	public:
		Vec3 normal;
		Vec3 point;
		Floor() {}
		Floor(const Vec3& _normal, const Vec3& _point) : normal(_normal), point(_point) {}
		bool intersection(Intersection& nearest, const ray& r) const
		{
			float NdotDir = Vec3::dot(normal, r.direction);
			if (abs(NdotDir) < 1e-7) return false;

			float t = Vec3::dot(point - r.origin, normal) / NdotDir;
			if (t < 0) return false;
			if (nearest.t < t) return false;
			nearest.t = t;
			nearest.point = r.point_at_parameter(t);
			nearest.normal = normal;
			return true;
		}
	};
}