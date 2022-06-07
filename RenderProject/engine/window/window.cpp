#include "window.h"

void Window::OnResize()
{
	GetClientRect(hwnd, &screen);
	/*image.resize((screen.right/2) * (screen.bottom/2));
	bitmap_info.bmiHeader.biWidth = screen.right/2;
	bitmap_info.bmiHeader.biHeight = screen.bottom/2;*/
	image.resize((screen.right) * (screen.bottom ));
	bitmap_info.bmiHeader.biWidth = screen.right ;
	bitmap_info.bmiHeader.biHeight = screen.bottom ;
}
