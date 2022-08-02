#pragma once
#include "../render/globals.hpp"
#include "Mesh.h"
#include "../math/Transform.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/Intersection.h"
#include "../objects/ObjRef.h"
#include "../math/Triangle.h"
#include "../render/Model.h"

class Cube
{
	bool math_intersection(math::Intersection& nearest, const ray& r) const;
public:
	Cube(){}
	static void Init();
	bool intersects(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest);
	static std::shared_ptr<Model> s_model;
	engine::ModelID ID;
};