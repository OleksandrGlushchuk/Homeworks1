#pragma once
#include "Scene.h"
#include "InputState.h"
#include "../math/camera.h"
#include "../math/ray.h"
#include "../objects/IntersectionQuery.h"
class Controller
{
	Window& wnd;
	Scene &scene;
	InputState input_state;
	ray ray_clicked_to_object;
	IntersectionQuery nearest_clicked_object;
	IntersectionQuery new_object_intersection;
	float distance_object_to_camera;
	bool need_to_rotate = false;
	bool need_to_move_object = false;
	float camera_move_offset_val = 3.f;
	float camera_angle_offset_val = 3.f;
	WORD mouse_x = 0, mouse_y = 0;
public:
	float delta_time;
	void RotateCamera();
	Controller(Window &_wnd, Scene &_scene): wnd(_wnd), scene(_scene){}
	void DrawScene();
	void ProcessInput();
	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);
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