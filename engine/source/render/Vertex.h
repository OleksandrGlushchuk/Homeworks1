#pragma once
#include "../math/vec3.h"
#include "../math/vec2.h"
struct Vertex
{
	Vec3 position;
	Vec2 textureCoords;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;
};