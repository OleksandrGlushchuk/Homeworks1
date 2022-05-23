#pragma once
#include "render.h"
#include "../math/camera.h"
class Controller
{
	Window& wnd;
	Render &render;
	WORD mouse_x=0, mouse_y=0;
	bool need_to_rotate = false;
	bool need_to_move_object = false;
public:
	void Rotate();
	Controller(Window &_wnd, Render &_render): wnd(_wnd), render(_render){}
	void ProcessInput();

	void OnLMouseDown(WORD x, WORD y);
	void OnLMouseMove(WORD x, WORD y);
	void OnLMouseUp(WORD x, WORD y);

	void OnRMouseDown(WORD x, WORD y);
	void OnRMouseMove(WORD x, WORD y);
	void OnRMouseUp(WORD x, WORD y);

	void OnChangeWindowSize();
	void moveCamera(const Vec3& offset, const Angles& angles);
};