#pragma once
#include <windows.h>
#include <vector>
class Window
{
	WNDCLASS wndclass;
	HDC device_context;
	RECT screen;
	BITMAPINFO bitmap_info;
	std::vector<RGBQUAD> image;
public:
	HWND hwnd;
	Window() = default;
	Window(LPCWSTR name, HINSTANCE hinstance, WNDPROC WndProc,  WNDCLASS _wndclass =
		{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		0, 0, 0, 0, 0,
		LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
	{
		wndclass.hInstance = hinstance;
		wndclass.lpfnWndProc = WndProc;
		RegisterClass(&wndclass);
		hwnd = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, 0, 0, wndclass.hInstance, 0);
		device_context = GetDC(hwnd);

		ZeroMemory(&bitmap_info, sizeof(BITMAPINFO));
		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biPlanes = 1;
	}
	LONG image_width() { 
		return bitmap_info.bmiHeader.biWidth;
	}
	LONG image_height() { 
		return bitmap_info.bmiHeader.biHeight;
	}
	void Show()
	{
		ShowWindow(hwnd, SW_SHOWNORMAL);
	}
	void OnResize(uint32_t image_compression = 2);
	friend class Scene;
	friend class Controller;
};