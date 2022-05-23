#pragma once
#include "ray.h"
#include "camera.h"
#include "Material.h"
class Floor
{
public:
	Vec3 normal;
	Vec3 point;
	Material mat;
	Floor() {}
	Floor(Vec3 _normal, Vec3 _point): normal(_normal), point(_point) {}

	bool intersection(Intersection& nearest, const ray& r) const
	{
		float NdotDir = dot(normal, r.direction);
		if (abs(NdotDir) < 1e-7) return false;

		float t = dot(point - r.origin, normal) / NdotDir;
		if (t < 0) return false;
		if (nearest.t < t) return false;
		nearest.t = t;
		nearest.point = r.point_at_parameter(t);
		nearest.normal = normal;
		nearest.mat = mat;
		return true;
	}
};