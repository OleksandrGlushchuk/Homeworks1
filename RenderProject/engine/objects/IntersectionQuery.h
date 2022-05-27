#pragma once
#include "../math/Intersection.h"
#include "Material.h"
#include <memory>
struct IntersectionQuery
{
	math::Intersection nearest;
	Material material;
	std::unique_ptr<IObjectMover> mover;
	void reset() 
	{
		nearest.reset();
		mover = nullptr;
	}
};