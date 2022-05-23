#pragma once
#include "vec3.h"
#include <cmath>
#include "Material.h"
//#include "Cube.h"
class Cube;
class Sphere;

class Intersection
{
public:
	float t;
	Vec3 point;
	Vec3 point_model;
	Vec3 normal;
	Material mat;
	void reset() { t = std::numeric_limits<float>::infinity(); }
	bool exists() const { return std::isfinite(t); }
	const Cube* cube = nullptr;
	const Sphere* sphere = nullptr;
	static Intersection infinite() { Intersection inf; inf.reset(); return inf; }
};