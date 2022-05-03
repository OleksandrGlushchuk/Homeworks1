#include "window/window.h"
#include "window/controller.h"
#include "window/render.h"
#include <thread>

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

bool FrameTimeElapsed(clock_t &start, float seconds)
{
	if (clock() - start >= seconds * CLOCKS_PER_SEC)
	{
		start = clock();
		return true;
	}
	return false;
}

Window window;
Render render;
Controller controller(render);


int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprev, _In_ LPWSTR cmdline, _In_ int cmdshow)
{
	window = Window(L"homework-1", hinstance, WndProc);
	window.Show();
	MSG msg;
	clock_t start = clock();
	while (true)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return TRUE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (FrameTimeElapsed(start,1.0 / 60.0))
		{
			controller.OnKeyDown();
			render.DrawOn(window);
		}
		std::this_thread::yield();
	}
	return TRUE;
}

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_SIZE: {
		controller.OnChangeWindowSize(window);
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