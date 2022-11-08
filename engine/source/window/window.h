#pragma once
#include <windows.h>
#include "../render/RenderTargetView.h"

namespace engine::windows
{
	class Window
	{
		HWND m_wndHandle = nullptr;
		DxResPtr<IDXGISwapChain1> m_swapChain1;
		DxResPtr<ID3D11Texture2D> m_backBuffer;
		D3D11_TEXTURE2D_DESC m_backbufferDesc;
		RenderTargetView m_renderTargetView;

		WNDCLASS wndclass;
		HDC device_context;
		RECT screen;
		void initSwapchain();
		void initBackBuffer();
		void initRenderTargetView();
	public:
		Window() {}
		Window(LPCWSTR name, HINSTANCE hinstance, WNDPROC WndProc, WNDCLASS _wndclass =
			{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			0, 0, 0, 0, 0,
			LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
		{
			wndclass.hInstance = hinstance;
			wndclass.lpfnWndProc = WndProc;
			RegisterClass(&wndclass);
			m_wndHandle = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, 0, 0, wndclass.hInstance, 0);
			device_context = GetDC(m_wndHandle);

			GetClientRect(m_wndHandle, &screen);
			initSwapchain();
			initBackBuffer();
			initRenderTargetView();
		}

		void Show()
		{
			ShowWindow(m_wndHandle, SW_SHOWNORMAL);
		}

		void BeginFrame();

		void EndFrame();

		void OnResize();

		friend class Application;
	};
}