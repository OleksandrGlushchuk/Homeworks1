#include "render.h"

void Render::Init(RECT screen)
{
	sp = sphere(vec3((screen.right) / 2.0, (screen.bottom) / 2.0, 60), 50);
	delete[] image;
	image = new RGBQUAD[screen.right * screen.bottom];
	ZeroMemory(&bitmap_info, sizeof(BITMAPINFO));
	bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info.bmiHeader.biBitCount = 4 * 8;
	bitmap_info.bmiHeader.biWidth = screen.right;
	bitmap_info.bmiHeader.biHeight = screen.bottom;
	bitmap_info.bmiHeader.biPlanes = 1;
}

void Render::Redraw(RECT screen, HDC device_context)
{
	ray r(vec3(0, 0, 0), vec3(0, 0, 1));
	hit_record record;
	//int size = screen.right * screen.bottom;
	int image_index;
	for (int y = 0; y < screen.bottom; y++)
	{
		for (int x = 0; x < screen.right; x++)
		{
			image_index =  screen.right * y + x; //if image_index is size - screen.right + screen.right * y + x there will be an error
			r.origin[0] = x; r.origin[1] = y;
			image[image_index].rgbBlue = 0;
			if (sp.hit(r,record))
			{
				image[image_index].rgbRed = 255;
				image[image_index].rgbGreen = 0;
			}
			else
			{
				image[image_index].rgbRed = 0;
				image[image_index].rgbGreen = 255;
			}
		}
	}
}

void Render::Draw(RECT screen, HDC device_context)
{
	if (need_to_redraw)
	{
		need_to_redraw = false;
		Redraw(screen, device_context);
	}
	SetDIBitsToDevice(device_context, 0, 0, screen.right, screen.bottom, 0, 0, 0, screen.bottom, image, &bitmap_info, DIB_RGB_COLORS);
}