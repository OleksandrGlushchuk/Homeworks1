#pragma once
#include "../math/Floor.h"
#include "../window/Scene.h"
#include "Material.h"
#include "ObjRef.h"
class Floor : public math::Floor
{
public:
	using math::Floor::Floor;
	Material material;
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
	{
		bool found = this->intersection(outNearest, _ray);
		if (found)
		{
			outRef.type = ObjRef::IntersectedType::FLOOR;
			outRef.object = this;
			outMaterial = material;
		}
		return found;
	}
};