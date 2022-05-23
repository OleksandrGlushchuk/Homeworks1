#pragma once
#include "window.h"
#include "controller.h"
#include "../math/camera.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/sphere.h"
#include "../math/Floor.h"
#include "../math/Cube.h"
#include "../math/Intersection.h"
#include "../math/Light.h"


class Render
{
	Camera camera;
	std::vector<Sphere> sp = std::vector<Sphere>(2);
	std::vector<Point_Light> light = std::vector<Point_Light>(2);
	std::vector<Sphere> light_sphere = std::vector<Sphere>(light.size()+1);
	std::vector<Cube> cube = std::vector<Cube>(2);
	Directional_Light dirlight;
	Spot_Light spotlight;
	bool need_to_redraw;
	Floor floor = Floor(Vec3(0, 1, 0), Vec3(0, -10, 0));
public:
	void Init(Window& wnd);
	bool FindIntersection(const ray& r, Intersection& nearest);
	void DrawNearest(Intersection& nearest, RGBQUAD &pixel);
	void Redraw(Window &wnd);
	void Draw(Window& wnd);

	friend class Controller;
};