#pragma once
struct ObjRef
{
	enum class IntersectedType { SPHERE, CUBE, FLOOR, SPHERE_POINT_LIGHT, SPHERE_SPOT_LIGHT, NONE };
	void* object;
	IntersectedType type;
};