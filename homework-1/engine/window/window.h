#pragma once
#include <windows.h>
#include "controller.h"
class Window
{
	HWND hwnd;
	WNDCLASS wndclass;
	static HDC device_context;
	static RECT screen;
	static Controller controller;
	static Render render;
public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	Window(LPCWSTR name, HINSTANCE hinstance, WNDCLASS _wndclass =
		{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		WndProc, 0, 0, 0, 0,
		LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
	{
		wndclass.hInstance = hinstance;
		RegisterClass(&wndclass);
		hwnd = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 320, 300, 0, 0, wndclass.hInstance, 0);
	}

	void Show();

	void Run();
};

