﻿#pragma once
#include "window.h"
#include "ParallelExecutor.h"

#include "../math/camera.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/Intersection.h"
#include "../math/Light.h"

#include "../objects/ObjectMover.h"
#include "../objects/Sphere.h"
#include "../objects/Floor.h"
#include "../objects/Cube.h"
#include "../objects/ObjRef.h"
#include "../objects/IntersectionQuery.h"

class Scene
{
	//ParallelExecutor executor = ParallelExecutor(ParallelExecutor::MAX_THREADS-1u);
	Camera camera;
	std::vector<Sphere> sp = std::vector<Sphere>(169);
	std::vector<Cube> cube = std::vector<Cube>(0);

	std::vector<Sphere_Point_Light> sphere_point_light = std::vector<Sphere_Point_Light>(2);
	std::vector<Sphere_Spot_Light> sphere_spot_light = std::vector<Sphere_Spot_Light>(1);
	std::vector<Directional_Light> dir_light = std::vector<Directional_Light>(1);

	bool need_to_redraw;
	bool smooth_reflection = false;
	Floor floor = Floor(Vec3(0, 1, 0), Vec3(0, -10, 0));
	Vec3 scene_ambient = Vec3(0.2f, 0.2f, 0.2f);
	Vec3 background_color = scene_ambient;
	float EV100 = 2;
public:
	void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, IntersectionQuery& query);

	Vec3 CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, math::Intersection& nearest, Material &nearest_material);
	Vec3 CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, math::Intersection& nearest, Material& nearest_material);
	Vec3 CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, math::Intersection& nearest, Material& nearest_material);

	void ComputePixelColor(Window& wnd, float x, float y);
	Vec3 CalculateLighting(const ray& ray_to_object, math::Intersection& nearest, Material& nearest_material, const int& depth);
	void СorrectLight(Vec3& light);
	Vec3 adjustExposure(const Vec3& color, float EV100);
	Vec3 acesHdr2Ldr(const Vec3& hdr);

	ray RayFromCameraTo(Window &wnd, float x, float y);

	void Redraw(Window& wnd);
	void Draw(Window& wnd);

	friend class Controller;
private:
	IntersectionQuery intersection_query;
};