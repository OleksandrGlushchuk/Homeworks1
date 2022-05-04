#include "window/window.h"
#include "window/controller.h"
#include "window/render.h"
#include <thread>
#include <chrono>
const float FRAME_DURATION = 1.f / 60.f;

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

bool FrameTimeElapsed(std::chrono::time_point<std::chrono::steady_clock> &start, const float &seconds)
{
	if (std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count() >= seconds)
	{
		start = std::chrono::steady_clock::now();
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
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	while (true)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return TRUE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (FrameTimeElapsed(start, FRAME_DURATION))
		{
			controller.ProcessInput();
			render.Draw(window);
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
		window.OnResize();
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