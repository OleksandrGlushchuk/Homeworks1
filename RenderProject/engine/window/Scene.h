#pragma once
#include "window.h"
#include "ParallelExecutor.h"

#include "../math/camera.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/Intersection.h"
#include "../math/Light.h"
#include "../math/LightingPostProcess.h"

#include "../objects/ObjectMover.h"
#include "../objects/Sphere.h"
#include "../objects/Floor.h"
#include "../objects/Cube.h"
#include "../objects/ObjRef.h"
#include "../objects/IntersectionQuery.h"

class Scene
{
	Camera camera;
	std::vector<Sphere> sp = std::vector<Sphere>(65);
	std::vector<Cube> cube = std::vector<Cube>(0);

	std::vector<Sphere_Point_Light> sphere_point_light = std::vector<Sphere_Point_Light>(2);
	std::vector<Sphere_Spot_Light> sphere_spot_light = std::vector<Sphere_Spot_Light>(1);
	std::vector<Directional_Light> dir_light = std::vector<Directional_Light>(1);

	bool need_to_redraw;
	bool smooth_reflection = false;
	Floor floor = Floor(Vec3(0, 1, 0), Vec3(0, -10, 0));
	Vec3 scene_ambient = Vec3(0.2f, 0.2f, 0.2f);
	Vec3 background_color = scene_ambient;
	float MAX_REFLECTIVE_ROUGHNESS = 0.1f;
	float EV100 = 2.f;
public:
	void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, IntersectionQuery& query);

	Vec3 CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, const Vec3& view_pos, math::Intersection& nearest, Material &nearest_material);
	Vec3 CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, const Vec3& view_pos, math::Intersection& nearest, Material& nearest_material);
	Vec3 CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, const Vec3& view_pos, math::Intersection& nearest, Material& nearest_material);

	void ComputePixelColor(Window& wnd, float x, float y);
	Vec3 CalculateLighting(const ray& ray_to_object, const Vec3& view_pos, math::Intersection& nearest, Material& nearest_material, const int& depth);
	bool find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest, Material& outMaterial);

	void LightingPostProcess(Vec3& light);

	ray RayFromCameraTo(Window &wnd, float x, float y);

	void Redraw(Window& wnd);
	void Draw(Window& wnd);

	friend class Controller;
private:
	IntersectionQuery intersection_query;
};