#include "Triangle.h"
#include "ray.h"

bool math::Triangle::intersection(math::Intersection& nearest, const ray& r) const
{
	float NdotDir = Vec3::dot(normal,r.direction);
	if (abs(NdotDir) < 1e-7)
		return false;

	float t = Vec3::dot(v0 - r.origin,normal) / NdotDir;

	if (t < 0)
		return false;

	if (t > nearest.t)
		return false;

	Vec3 P = r.point_at_parameter(t);
	Vec3 C;

	// edge 0
	Vec3 edge0 = v1 - v0;
	Vec3 vp0 = P - v0;
	C = Vec3::cross(edge0,vp0);
	if (Vec3::dot(normal,C) < 0)
		return false;

	// edge 1
	Vec3 edge1 = v2 - v1;
	Vec3 vp1 = P - v1;
	C = Vec3::cross(edge1,vp1);
	if (Vec3::dot(normal, C) < 0)
		return false;

	// edge 2
	Vec3 edge2 = v0 - v2;
	Vec3 vp2 = P - v2;
	C = Vec3::cross(edge2,vp2);
	if (Vec3::dot(normal, C) < 0)
		return false;

	nearest.point = P;
	nearest.t = t;

	nearest.normal = normal;
	return true;

}

bool math::Triangle::intersection(engine::MeshIntersection& nearest, const ray& r)
{
	float NdotDir = Vec3::dot(normal, r.direction);
	if (abs(NdotDir) < 1e-7)
		return false;

	float t = Vec3::dot(v0 - r.origin, normal) / NdotDir;

	if (t < 0)
		return false;

	if (t > nearest.t)
		return false;

	Vec3 P = r.point_at_parameter(t);
	Vec3 C;

	// edge 0
	Vec3 edge0 = v1 - v0;
	Vec3 vp0 = P - v0;
	C = Vec3::cross(edge0, vp0);
	if (Vec3::dot(normal, C) < 0)
		return false;

	// edge 1
	Vec3 edge1 = v2 - v1;
	Vec3 vp1 = P - v1;
	C = Vec3::cross(edge1, vp1);
	if (Vec3::dot(normal, C) < 0)
		return false;

	// edge 2
	Vec3 edge2 = v0 - v2;
	Vec3 vp2 = P - v2;
	C = Vec3::cross(edge2, vp2);
	if (Vec3::dot(normal, C) < 0)
		return false;

	nearest.pos = P;
	nearest.t = t;

	nearest.normal = normal;
	return true;
}
