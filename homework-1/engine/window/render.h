#pragma once
#include "window.h"
#include "controller.h"
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/sphere.h"

class Render
{
	sphere sp;
	bool need_to_redraw;
public:
	void Init(Window& wnd);
	void RedrawOn(Window &wnd);
	void DrawOn(Window& wnd);

	friend class Controller;
};