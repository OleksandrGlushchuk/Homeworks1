#pragma once
#include "render.h"
class Controller
{
	Render &render;
public:
	Controller(Render &_render): render(_render){}
	void OnKeyDown();
	void OnMouseMove(WORD x, WORD y);
	void OnChangeWindowSize(RECT screen, HDC device_context);
};