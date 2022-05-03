#include "window.h"
#include "render.h"
#include <thread>
HDC Window::device_context;
RECT Window::screen;
Render Window::render;
Controller Window::controller = Window::render;
LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
		device_context = GetDC(hwnd);
		break;
	case WM_SIZE: {
		GetClientRect(hwnd, &screen);
		controller.OnChangeWindowSize(screen, device_context);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		if (wparam == MK_RBUTTON)
			controller.OnMouseMove(LOWORD(lparam), HIWORD(lparam));
		break;
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return TRUE;
}

void Window::Show()
{
	ShowWindow(hwnd, SW_SHOWNORMAL);
}

bool FrameTimeElapsed(double seconds)
{
	static time_t start = clock();
	if (clock() - start >= seconds*CLOCKS_PER_SEC)
	{
		start = clock();
		return true;
	}
	return false;
}

void Window::Run()
{
	MSG msg;
	while (true)
	{

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		}
		controller.OnKeyDown();
		if(FrameTimeElapsed(1.0/60.0))
			render.Draw(screen, device_context);
		std::this_thread::yield();
	}
}
