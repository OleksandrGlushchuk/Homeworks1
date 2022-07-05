#include "window/window.h"
#include "window/controller.h"
#include "window/Scene.h"
#include "window/Timer.h"
#include <thread>
#include <chrono>
#include "render/globals.hpp"
const float FRAME_DURATION = 1.f / 60.f;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


engine::Globals global;
engine::windows::WindowD3D window;
engine::windows::SceneD3D scene;
engine::windows::ControllerD3D controller(window, scene);

int WINAPI wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hprev, _In_ LPWSTR cmdline, _In_ int cmdshow)
{
	global.initD3D();

	window = engine::windows::WindowD3D(L"homework-4", hinstance, WndProc);
	controller.InitScene();
	window.Show();

	MSG msg;
	//Timer timer;
	//timer.start_timer();
	//float delta_time;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) return TRUE;
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		/*delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count();
		if (timer.FrameTimeElapsed(FRAME_DURATION))
		{

			window.BeginFrame();

			window.EndFrame();
		}
		delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(timer.start.time_since_epoch()).count() - delta_time + 0.01f;*/
		window.BeginFrame();
		controller.DrawScene();
		window.EndFrame();
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

	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}