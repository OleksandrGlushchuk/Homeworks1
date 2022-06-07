#pragma once
#include "../math/sphere.h"
#include "../math/Light.h"
#include "../window/Scene.h"
#include "Material.h"
#include "ObjRef.h"
class Sphere : public math::Sphere
{
public:
	using math::Sphere::Sphere;
	Material material;
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
	{
		bool found = intersection(outNearest, _ray);
		if (found)
		{
			outRef.type = ObjRef::IntersectedType::SPHERE;
			outRef.object = this;
			outMaterial = material;
		}
		return found;
	}
};

class Sphere_Point_Light
{
public:
	Sphere sphere;
	Point_Light light;
	Sphere_Point_Light() {}
	Sphere_Point_Light(const Vec3& pos, float radius) : sphere(pos, radius)
	{
		light.pos = pos;
		light.light_radius = radius;
		sphere.material.emmission = Vec3(1, 1, 1);
		sphere.material.only_emmission = true;
	}
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
	{
		bool found = sphere.intersection(outNearest, _ray);
		if (found)
		{
			outRef.type = ObjRef::IntersectedType::SPHERE_POINT_LIGHT;
			outRef.object = this;
			outMaterial = sphere.material;
		}
		return found;
	}
};

class Sphere_Spot_Light
{
public:
	Sphere sphere;
	Spot_Light light;
	Sphere_Spot_Light() {}
	Sphere_Spot_Light(const Vec3& pos, float radius, const Vec3& direction) : sphere(pos, radius)
	{
		light.light_radius = radius;
		light.direction = direction;
		light.pos = pos;
		sphere.material.emmission = Vec3(1, 1, 1);
		sphere.material.only_emmission = true;
	}
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
	{
		bool found = sphere.intersection(outNearest, _ray);
		if (found)
		{
			outRef.type = ObjRef::IntersectedType::SPHERE_SPOT_LIGHT;
			outRef.object = this;
			outMaterial = sphere.material;
		}
		return found;
	}
};