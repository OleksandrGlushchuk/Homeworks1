#pragma once
#include "vec3.h"
class Material
{
public:
	Vec3 albedo;
	Vec3 emmission;
	float specular;
	float glossiness;
	bool only_emmission;
};