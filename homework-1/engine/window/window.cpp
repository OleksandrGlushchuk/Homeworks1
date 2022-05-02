#include "window.h"
#include "render.h"
HDC Window::device_context, Window::compatible_context;
RECT Window::screen;
HBITMAP Window::compatible_bitmap;

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
		device_context = GetDC(hwnd);
		compatible_context = CreateCompatibleDC(device_context);
		break;
	case WM_SIZE:
		GetClientRect(hwnd, &screen);
		compatible_bitmap = CreateCompatibleBitmap(device_context, screen.right, screen.bottom);
		Render::Input_processing::OnChangeWindowSize(screen, device_context, compatible_context, compatible_bitmap);
		break;
	case WM_DESTROY:
		DeleteObject(compatible_bitmap);
		DeleteDC(compatible_context);
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		if(wparam == MK_RBUTTON)
			Render::Input_processing::OnMouseMove(LOWORD(lparam), HIWORD(lparam));
		break;
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return TRUE;
}

void Window::Show()
{
	ShowWindow(hwnd, SW_SHOWNORMAL);
}

void Window::Run()
{
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Render::Input_processing::OnKeyDown();
		Render::Draw(screen, device_context, compatible_context, compatible_bitmap);
		Sleep(16);
	}
}
