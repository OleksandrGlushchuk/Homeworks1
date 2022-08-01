#pragma once
#include "window.h"
#include "../math/camera.h"
#include "../math/ray.h"
#include "../objects/IntersectionQuery.h"

#include "../objects/Triangle.h"
#include "../objects/Cube.h"
#include "../objects/Sky.h"
#include "../objects/Knight.h"
#include "../objects/Samurai.h"
#include <vector>

namespace engine::windows
{
	class Scene
	{
		Camera camera;
		std::vector<Cube> cube;
		std::vector<Knight> knight;
		std::vector<Samurai> samurai;
		Sky sky;
	public:
		void Draw();

		void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest);
		bool findIntersection(const ray& _ray, IntersectionQuery& query);
		friend class Controller;
	};
}