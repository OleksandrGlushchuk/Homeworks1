#pragma once
#include "../math/Cube.h"
#include "../window/Scene.h"
#include "Material.h"
#include "ObjRef.h"
class Cube : public math::Cube
{
public:
	using math::Cube::Cube;
	Material material;
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
	{
		bool found = intersection(outNearest, _ray);
		if (found)
		{
			outRef.type = ObjRef::IntersectedType::CUBE;
			outRef.object = this;
			outMaterial = material;
		}
		return found;
	}
};