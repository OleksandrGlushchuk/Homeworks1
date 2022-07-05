#pragma once
#include <windows.h>

//#include <vector>
//class Window
//{
//	WNDCLASS wndclass;
//	HDC device_context;
//	RECT screen;
//	BITMAPINFO bitmap_info;
//	std::vector<RGBQUAD> image;
//public:
//	HWND hwnd;
//	Window() = default;
//	Window(LPCWSTR name, HINSTANCE hinstance, WNDPROC WndProc,  WNDCLASS _wndclass =
//		{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
//		0, 0, 0, 0, 0,
//		LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
//	{
//		wndclass.hInstance = hinstance;
//		wndclass.lpfnWndProc = WndProc;
//		RegisterClass(&wndclass);
//		hwnd = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, 0, 0, wndclass.hInstance, 0);
//		device_context = GetDC(hwnd);
//
//		ZeroMemory(&bitmap_info, sizeof(BITMAPINFO));
//		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//		bitmap_info.bmiHeader.biBitCount = 32;
//		bitmap_info.bmiHeader.biPlanes = 1;
//	}
//	LONG image_width() { 
//		return bitmap_info.bmiHeader.biWidth;
//	}
//	LONG image_height() { 
//		return bitmap_info.bmiHeader.biHeight;
//	}
//	void Show()
//	{
//		ShowWindow(hwnd, SW_SHOWNORMAL);
//	}
//	void OnResize(uint32_t image_compression = 2);
//	friend class Scene;
//	friend class Controller;
//};

#include "../render/DxRes.hpp"

namespace engine::windows
{
	class WindowD3D
	{
		HWND m_wndHandle = nullptr;
		DxResPtr<IDXGISwapChain1> m_swapChain1;
		DxResPtr<ID3D11Texture2D> m_backBuffer;
		D3D11_TEXTURE2D_DESC m_backbufferDesc;
		DxResPtr<ID3D11RenderTargetView1> m_renderTargetView1;

		WNDCLASS wndclass;
		HDC device_context;
		RECT screen;
		void initSwapchain();
		void initBackBuffer();
		void initRenderTargetView();
	public:
		WindowD3D() {}
		WindowD3D(LPCWSTR name, HINSTANCE hinstance, WNDPROC WndProc, WNDCLASS _wndclass =
			{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			0, 0, 0, 0, 0,
			LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
		{
			wndclass.hInstance = hinstance;
			wndclass.lpfnWndProc = WndProc;
			RegisterClass(&wndclass);
			m_wndHandle = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, 0, 0, wndclass.hInstance, 0);
			device_context = GetDC(m_wndHandle);

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

	};
}