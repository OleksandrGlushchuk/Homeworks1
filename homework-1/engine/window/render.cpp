#include "render.h"


sphere Render::sp;
bool Render::need_to_redraw;

void Render::Init(RECT screen)
{
	sp = sphere(vec3((screen.right) / 2.0, (screen.bottom) / 2.0, 60), 50);
}

void Render::Redraw(RECT screen, HDC compatible_context, HBITMAP compatible_bitmap)
{
	ray r(vec3(0, 0, 0), vec3(0, 0, 1));
	hit_record record;
	SelectObject(compatible_context, compatible_bitmap);
	SelectObject(compatible_context, GetStockObject(DC_BRUSH));
	SetDCBrushColor(compatible_context, RGB(0, 255, 0));
	SelectObject(compatible_context, GetStockObject(DC_PEN));
	SetDCPenColor(compatible_context, RGB(0, 255, 0));
	Rectangle(compatible_context, 0, 0, screen.right, screen.bottom);
	for (int y = 0; y < screen.bottom; y++)
	{
		for (int x = 0; x < screen.right; x++)
		{
			r.origin[0] = x; r.origin[1] = y;
			if (sp.hit(r,record))
			{
				SetPixel(compatible_context, x, y, RGB(255, 0, 0));
			}
		}
	}
}

void Render::Draw(RECT screen, HDC device_context, HDC compatible_context, HBITMAP compatible_bitmap)
{
	if (need_to_redraw)
	{
		need_to_redraw = false;
		Redraw(screen, compatible_context, compatible_bitmap);
	}
	BitBlt(device_context, 0, 0, screen.right, screen.bottom, compatible_context, 0, 0, SRCCOPY);
}

void Render::Input_processing::OnKeyDown()
{
	if (GetKeyState('W') < 0)
	{
		need_to_redraw = true;
		sp.center[1] -= 5;	
	}
	if (GetKeyState('S') < 0)
	{
		need_to_redraw = true;
		sp.center[1] += 5;
	}
	if (GetKeyState('A') < 0)
	{
		need_to_redraw = true;
		sp.center[0] -= 5;
	}
	if (GetKeyState('D') < 0)
	{
		need_to_redraw = true;
		sp.center[0] += 5;
	}
}

void Render::Input_processing::OnMouseMove(WORD x, WORD y)
{
	sp.center[0] = x;
	sp.center[1] = y;
	need_to_redraw = true;
}

void Render::Input_processing::OnChangeWindowSize(RECT screen, HDC device_context, HDC compatible_context, HBITMAP compatible_bitmap)
{
	Render::Init(screen);
	Render::Redraw(screen, compatible_context, compatible_bitmap);
	Render::Draw(screen, device_context, compatible_context, compatible_bitmap);
}