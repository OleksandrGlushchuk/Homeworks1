#pragma once
#include <windows.h>
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
		DxResPtr<ID3D11Texture2D> m_depthStencilBuffer;
		DxResPtr<ID3D11DepthStencilState> m_depthStencilState;
		DxResPtr<ID3D11DepthStencilView> m_depthStencilView;

		WNDCLASS wndclass;
		HDC device_context;
		RECT screen;
		void initSwapchain();
		void initBackBuffer();
		void initRenderTargetView();

		void initDepthStencilResource();
		void initDepthStencilState();
		void initDepthStencilView();

		void initDepthStencilBuffer();
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
			m_wndHandle = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 900, 900, 0, 0, wndclass.hInstance, 0);
			device_context = GetDC(m_wndHandle);

			GetClientRect(m_wndHandle, &screen);
			initSwapchain();
			initBackBuffer();
			initRenderTargetView();
			initDepthStencilBuffer();
		}

		void Show()
		{
			ShowWindow(m_wndHandle, SW_SHOWNORMAL);
		}

		void BeginFrame();

		void EndFrame();

		void OnResize();

		friend class ControllerD3D;
	};
}