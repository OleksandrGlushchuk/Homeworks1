#include "window/window.h"
#include "window/controller.h"
#include "window/Scene.h"
//#include "window/Window.hpp"
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
	window = Window(L"homework-3", hinstance, WndProc);
	controller.InitScene();
	window.Show();
	
	/*engine::windows::Window window(L"homework-4", hinstance, WndProc);
	window.Show();*/

	MSG msg;
	Timer timer;
	timer.start_timer();
	float delta_time;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return TRUE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count();
		if (timer.FrameTimeElapsed(FRAME_DURATION))
		{
			controller.DrawScene();

		}
		delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count() - delta_time + 0.01f;
		controller.delta_time = delta_time;

		/*window.BeginFrame();


		window.EndFrame();*/

		std::this_thread::yield();
	}
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_SIZE:
		controller.OnChangeWindowSize();
		break;
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
		if (wparam == MK_LBUTTON || wparam == (MK_LBUTTON | MK_RBUTTON))
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

	case WM_KEYDOWN:
		if((HIWORD(lparam) & KF_REPEAT) != KF_REPEAT)
			controller.OnKeyDown(wparam);
		break;

	case WM_KEYUP:
		controller.OnKeyUp(wparam);
		break;

	case WM_MOUSEWHEEL:
		controller.OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wparam));
		break;

	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}