#pragma once
#include <windows.h>
#include "../math/vec3.h"
#include "../math/ray.h"
#include "../math/sphere.h"
class Render
{
	static sphere sp;
	static bool need_to_redraw;
public:
	static void Init(RECT screen);
	static void Redraw(RECT screen, HDC compatible_context, HBITMAP compatible_bitmap);
	static void Draw(RECT screen, HDC device_context, HDC compatible_context, HBITMAP compatible_bitmap);

	class Input_processing
	{
	public:
		static void OnKeyDown();
		static void OnMouseMove(WORD x, WORD y);
		static void OnChangeWindowSize(RECT screen, HDC device_context, HDC compatible_context, HBITMAP compatible_bitmap);
	};
};