#pragma once
#include "Scene.h"
#include "../math/camera.h"
#include "../math/ray.h"
#include "../objects/IntersectionQuery.h"
class Controller
{
	Window& wnd;
	Scene &scene;
	WORD mouse_x=0, mouse_y=0;
	ray ray_clicked_to_object;
	IntersectionQuery nearest_clicked_object;
	IntersectionQuery new_object_intersection;
	float distance_object_to_camera;
	float camera_move_offset_val = 3.f;
	float camera_angle_offset_val = 3.f;
	bool need_to_rotate = false;
	bool need_to_move_object = false;
public:
	void RotateCamera();
	Controller(Window &_wnd, Scene &_scene): wnd(_wnd), scene(_scene){}
	void ProcessInput();

	void OnLMouseDown(WORD x, WORD y);
	void OnLMouseMove(WORD x, WORD y);
	void OnLMouseUp(WORD x, WORD y);

	void OnRMouseDown(WORD x, WORD y);
	void OnRMouseMove(WORD x, WORD y);
	void OnRMouseUp(WORD x, WORD y);

	void OnChangeWindowSize();
	void moveCamera(const Vec3& offset, const Angles& angles);
	void InitScene();
};