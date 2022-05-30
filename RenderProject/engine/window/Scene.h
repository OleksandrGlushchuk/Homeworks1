#pragma once
#include "window.h"

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
	Camera camera;
	std::vector<Sphere> sp = std::vector<Sphere>(2);
	std::vector<Cube> cube = std::vector<Cube>(4);

	std::vector<Sphere_Point_Light> sphere_point_light = std::vector<Sphere_Point_Light>(2);
	std::vector<Sphere_Spot_Light> sphere_spot_light = std::vector<Sphere_Spot_Light>(1);
	std::vector<Directional_Light> dir_light = std::vector<Directional_Light>(1);

	bool need_to_redraw;
	Floor floor = Floor(Vec3(0, 1, 0), Vec3(0, -10, 0));
	Vec3 background_color = Vec3(60, 60, 170);
	Vec3 scene_ambient = Vec3(20, 20, 20);
public:
	void DrawNearest(math::Intersection& nearest, Material &nearest_material, RGBQUAD& pixel);
	void Redraw(Window &wnd);
	void Draw(Window& wnd);

	void findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial);
	bool findIntersection(const ray& _ray, IntersectionQuery& query);

	Vec3 CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, math::Intersection& nearest, Material &nearest_material);
	Vec3 CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, math::Intersection& nearest, Material& nearest_material);
	Vec3 CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, math::Intersection& nearest, Material& nearest_material);

	friend class Controller;
private:
	IntersectionQuery intersection_query;
};