#include "window/Application.h"
#include "math/Timer.h"
#include <thread>
#include <chrono>
#include "render/singletones/globals.hpp"
const float FRAME_DURATION = 1.f / 60.f;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

engine::windows::Window window;
engine::windows::Renderer renderer;
engine::windows::Application application(window, renderer);

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprev, _In_ LPWSTR cmdline, _In_ int cmdshow)
{
	engine::init();

	window = engine::windows::Window(L"homework-10", hinstance, WndProc);
	application.Init();
	window.Show();

	MSG msg;
	Timer timer;
	timer.StartTimer();
	float delta_time = 0;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				engine::deinit();
				return msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		
		if (timer.TimeElapsed(FRAME_DURATION))
		{
			delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count();
			application.Draw();
			application.delta_time = std::max(std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count() - delta_time, FRAME_DURATION);
		}
		
		
		std::this_thread::yield();
	}
	engine::deinit();
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{

	case WM_ENTERSIZEMOVE:
		application.OnEnterSizeMove();
		break;

	case WM_EXITSIZEMOVE:
		application.OnExitSizeMove();
		break;

	case WM_SIZE:
		application.OnChangeWindowSize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if ((HIWORD(lparam) & KF_REPEAT) != KF_REPEAT)
			application.OnKeyDown(wparam);
		break;
	case WM_KEYUP:
		application.OnKeyUp(wparam);
		break;
	case WM_LBUTTONDOWN:
		application.OnLMouseDown(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_MOUSEMOVE:
		if (wparam == MK_LBUTTON || wparam == (MK_LBUTTON | MK_RBUTTON) || wparam == (MK_LBUTTON | MK_SHIFT) || wparam == (MK_LBUTTON | MK_SHIFT | MK_RBUTTON))
			application.OnLMouseMove(LOWORD(lparam), HIWORD(lparam));
		if (wparam == MK_RBUTTON || wparam == (MK_RBUTTON | MK_LBUTTON) || wparam == (MK_RBUTTON | MK_SHIFT) || wparam == (MK_RBUTTON | MK_SHIFT | MK_LBUTTON))
			application.OnRMouseMove(LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_LBUTTONUP:
		application.OnLMouseUp(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_RBUTTONDOWN:
		application.OnRMouseDown(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_RBUTTONUP:
		application.OnRMouseUp(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_MOUSEWHEEL:
		application.OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wparam));
		break;
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}