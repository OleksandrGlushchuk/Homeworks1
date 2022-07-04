#include "../render/DxRes.hpp"
#include "../render/globals.hpp"

namespace engine::windows
{
	class Window
	{
		HWND m_wndhandle = nullptr;
		//DxResPtr<IDXGISwapChain1> m_swapchain;

		DxResPtr<IDXGISwapChain> m_swapChain;
		DxResPtr<ID3D11Texture2D> m_backBuffer;
		DxResPtr<ID3D11Device> m_device;
		DxResPtr<ID3D11DeviceContext> m_deviceContext;


		D3D11_TEXTURE2D_DESC m_backbufferDesc;



		DxResPtr<ID3D11RenderTargetView> m_renderTargetView;

		WNDCLASS wndclass;
		HDC device_context;
		RECT screen;

		void CreateDevice();
		

		void initSwapchain();
		void initBackBuffer();
		void initRenderTargetView();

	public:
		Window() = default;
		Window(LPCWSTR name, HINSTANCE hinstance, WNDPROC WndProc, WNDCLASS _wndclass =
			{ CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			0, 0, 0, 0, 0,
			LoadCursor(NULL, IDC_ARROW), 0, 0, L"MyClass" }) : wndclass(_wndclass)
		{
			wndclass.hInstance = hinstance;
			wndclass.lpfnWndProc = WndProc;
			RegisterClass(&wndclass);
			m_wndhandle = CreateWindow(wndclass.lpszClassName, name, WS_OVERLAPPEDWINDOW, 0, 0, 400, 400, 0, 0, wndclass.hInstance, 0);
			device_context = GetDC(m_wndhandle);

			
			/*initSwapchain();
			initBackBuffer();
			initRenderTargetView();*/

			CreateDevice();
			initRenderTargetView();
		}

		void Show()
		{
			ShowWindow(m_wndhandle, SW_SHOWNORMAL);
		}

		void BeginFrame();

		void EndFrame();
	};
}