#pragma once
#include <windows.h>
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/sphere.h"
class Render
{
	sphere sp;
	bool need_to_redraw;
	BITMAPINFO bitmap_info;
	RGBQUAD* image = nullptr;
public:
	void Init(RECT screen);
	void Redraw(RECT screen, HDC device_context);
	void Draw(RECT screen, HDC device_context);
	~Render()
	{
		delete[] image;
	}
	friend class Controller;
};