#include "window/window.h"
#include "window/controller.h"
#include "window/Scene.h"
#include "window/Timer.h"
#include <thread>
#include <chrono>
const float FRAME_DURATION = 1.f / 60.f;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

Window window;
Scene scene;
Controller controller(window,scene);


int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprev, _In_ LPWSTR cmdline, _In_ int cmdshow)
{
	window = Window(L"homework-2", hinstance, WndProc);
	window.Show();
	MSG msg;
	Timer timer;
	timer.start_timer();
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return TRUE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		if (timer.FrameTimeElapsed(FRAME_DURATION))
		{
			controller.ProcessInput();
			scene.Draw(window);
		}
		std::this_thread::yield();
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_SIZE: {
		controller.OnChangeWindowSize();
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		controller.OnLMouseDown(LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_RBUTTONDOWN:
		controller.OnRMouseDown(LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_MOUSEMOVE:
		if (wparam == MK_LBUTTON)
			controller.OnLMouseMove(LOWORD(lparam), HIWORD(lparam));
		if (wparam == MK_RBUTTON)
			controller.OnRMouseMove(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_LBUTTONUP:
		controller.OnLMouseUp(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_RBUTTONUP:
		controller.OnRMouseUp(LOWORD(lparam), HIWORD(lparam));
		break;
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return TRUE;
}