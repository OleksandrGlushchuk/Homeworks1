#pragma once
#include "../math/vec3.h"
class Material
{
public:
	bool only_emmission;
	Vec3 albedo;
	Vec3 emmission;
	float roughness;
	float metallness;
	Vec3 F0;
};