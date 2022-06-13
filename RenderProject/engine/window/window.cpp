#include "window.h"

void Window::OnResize(uint32_t image_compression)
{
	GetClientRect(hwnd, &screen);
	image.resize((screen.right/ image_compression) * (screen.bottom/ image_compression));
	bitmap_info.bmiHeader.biWidth = screen.right/ image_compression;
	bitmap_info.bmiHeader.biHeight = screen.bottom/ image_compression;
}
