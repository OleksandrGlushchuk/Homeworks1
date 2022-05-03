#include "render.h"
void Render::Init(Window& wnd)
{
	sp = sphere(vec3((wnd.screen.right) / 2.0, (wnd.screen.bottom) / 2.0, 60), 50);
	wnd.image.resize(wnd.screen.right * wnd.screen.bottom);
	wnd.bitmap_info.bmiHeader.biWidth = wnd.screen.right;
	wnd.bitmap_info.bmiHeader.biHeight = wnd.screen.bottom;
}

void Render::RedrawOn(Window& wnd)
{
	ray r(vec3(0, 0, 0), vec3(0, 0, 1));
	hit_record record;
	int size = wnd.screen.right * wnd.screen.bottom;
	int image_index;
	for (int y = 0; y < wnd.screen.bottom; y++)
	{
		for (int x = 0; x < wnd.screen.right; x++)
		{
			image_index = size- wnd.screen.right - wnd.screen.right *y + x;
			r.origin[0] = x; r.origin[1] = y;
			wnd.image[image_index].rgbBlue = 0;
			if (sp.hit(r,record))
			{
				wnd.image[image_index].rgbRed = 255;
				wnd.image[image_index].rgbGreen = 0;
			}
			else
			{
				wnd.image[image_index].rgbRed = 0;
				wnd.image[image_index].rgbGreen = 255;
			}
		}
	}
}

void Render::DrawOn(Window& wnd)
{
	if (need_to_redraw)
	{
		need_to_redraw = false;
		RedrawOn(wnd);
	}
	SetDIBitsToDevice(wnd.device_context, 0, 0, wnd.screen.right, wnd.screen.bottom, 
		0, 0, 0, wnd.screen.bottom, &wnd.image[0], &wnd.bitmap_info, DIB_RGB_COLORS);
}
