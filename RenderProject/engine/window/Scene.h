#pragma once
#include "window.h"
#include "../math/camera.h"
#include "../math/ray.h"
#include "../objects/IntersectionQuery.h"

#include "../objects/Triangle.h"
#include "../objects/Cube.h"
#include "../objects/Sky.h"
#include <vector>

namespace engine::windows
{
	class Scene
	{
		Camera camera;
		std::vector<Cube> cubes;
		Sky sky;
		//Triangle<MeshType::TexturedVertex3D> triangle;
	public:
		void Draw();

		void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest);
		bool findIntersection(const ray& _ray, IntersectionQuery& query);
		friend class Controller;
	};
}