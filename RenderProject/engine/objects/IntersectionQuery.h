#pragma once
#include "../math/Intersection.h"
#include "../objects/ObjectMover.h"
#include "../objects/ObjectRotator.h"
#include "Material.h"
#include <memory>
struct IntersectionQuery
{
	math::Intersection nearest;
	Material material;
	std::unique_ptr<IObjectMover> mover;
	std::unique_ptr<IObjectRotator> rotator;
	void reset() 
	{
		nearest.reset();
		mover = nullptr;
		rotator = nullptr;
	}
};