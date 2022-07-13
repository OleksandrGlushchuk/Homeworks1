#pragma once
#include "window.h"
#include "../math/camera.h"
#include "../math/ray.h"
#include "../objects/IntersectionQuery.h"

#include "../d3dobjects/TriangleD3D.h"
#include "../d3dobjects/CubeD3D.h"
#include "../d3dobjects/Sky.h"
#include <vector>

//class Scene
//{
//	Camera camera;
//	std::vector<Sphere> sp = std::vector<Sphere>(1);
//	std::vector<Cube> cube = std::vector<Cube>(4+3);
//
//	std::vector<Sphere_Point_Light> sphere_point_light = std::vector<Sphere_Point_Light>(0);
//	std::vector<Sphere_Spot_Light> sphere_spot_light = std::vector<Sphere_Spot_Light>(1);
//	std::vector<Directional_Light> dir_light = std::vector<Directional_Light>(0);
//
//	bool need_to_redraw;
//	bool smooth_reflection = false;
//	bool global_illumination = false;
//	Floor floor = Floor(Vec3(0, 1, 0), Vec3(0, -10, 0));
//	Vec3 scene_ambient = Vec3(0.1f, 0.1f, 0.1f);
//	Vec3 background_color = scene_ambient;
//
//	int MAX_DEPTH_FOR_SMOOTH_REFLECTION = 50;
//	int NUMBER_OF_SAMPLES = 1000;
//	float MAX_REFLECTIVE_ROUGHNESS = 0.1f;
//	float EV100 = 2.f;
//public:
//	void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial);
//	bool findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial, ObjRef& outRef);
//	bool findIntersection(const ray& _ray, IntersectionQuery& query);
//	bool find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest, Material& outMaterial);
//	bool find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest);
//
//	Vec3 CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, const Vec3& view_pos, const math::Intersection& nearest, const Material &nearest_material);
//	Vec3 CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material);
//	Vec3 CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material);
//
//	void ComputePixelColor(Window& wnd, float x, float y);
//	Vec3 CalculateLighting(const ray& ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material, const int& smooth_reflection_depth);
//	Vec3 CalculateLightingWithoutGI(const ray& ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material, const int& smooth_reflection_depth);
//	Vec3 CalculateGlobalIllumination(const int number_of_samples, const ray& ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material);
//	
//	
//	void LightingPostProcess(Vec3& light);
//
//	ray RayFromCameraTo(Window &wnd, float x, float y);
//
//	void Redraw(Window& wnd);
//	void Draw(Window& wnd);
//
//	friend class Controller;
//private:
//	IntersectionQuery intersection_query;
//};

namespace engine::windows
{
	class SceneD3D
	{
		std::vector<Cube> cubes;
		Sky sky;
		Camera camera;
		bool need_to_redraw;
	public:
		void Redraw();
		void Draw();

		void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest);
		bool findIntersection(const ray& _ray, IntersectionQuery& query);
		friend class ControllerD3D;
	};
}