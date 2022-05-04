#pragma once
#include "render.h"
class Controller
{
	Render &render;
public:
	Controller(Render &_render): render(_render){}
	void ProcessInput();
	void OnMouseMove(WORD x, WORD y);
	void OnChangeWindowSize(Window& wnd);
};