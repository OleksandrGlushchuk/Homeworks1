#include "Window.hpp"

namespace engine::windows
{
	void Window::initSwapchain()
	{
		//DXGI_SWAP_CHAIN_DESC1 desc;

		//// clear out the struct for use
		//memset(&desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

		//// fill the swap chain description struct
		//desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		//desc.BufferCount = 2;
		//desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//desc.Flags = 0;
		//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//desc.SampleDesc.Count = 1;                               // how many multisamples
		//desc.SampleDesc.Quality = 0;                             // ???
		//desc.Scaling = DXGI_SCALING_NONE;
		//desc.Stereo = false;
		//desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		//HRESULT res = s_factory->CreateSwapChainForHwnd(s_device, m_wndhandle, &desc, NULL, NULL, m_swapchain.reset());
		//ALWAYS_ASSERT(res >= 0 && "CreateSwapChainForHwnd");
	}

	void Window::CreateDevice()
	{
		// Define our swap chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = this->m_wndhandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = true;

		// Create the swap chain, device and device context
		auto result = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			nullptr, 0, D3D11_SDK_VERSION,
			&swapChainDesc, m_swapChain.reset(),
			m_device.reset(), nullptr, m_deviceContext.reset());

		// Check for error
		if (result != S_OK) {
			MessageBox(nullptr, L"Error creating DX11", L"Error", MB_OK);
			exit(0);
		}
	}

	void Window::initBackBuffer() // may be called after resizing
	{
		/*if (m_backbuffer.valid())
		{
			m_backbuffer.release();
			m_swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		}

		HRESULT result = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_backbuffer.reset());
		ALWAYS_ASSERT(result >= 0);

		ID3D11Texture2D* pTextureInterface = 0;
		m_backbuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		pTextureInterface->GetDesc(&m_backbufferDesc);
		pTextureInterface->Release();*/
	}


	void Window::initRenderTargetView()
	{
		//s_device->CreateRenderTargetView1(m_backbuffer, nullptr, m_renderTargetView.reset());
		ID3D11Texture2D* backBuffer;
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
		m_device->CreateRenderTargetView(backBuffer, nullptr, m_renderTargetView.reset());
		backBuffer->GetDesc(&m_backbufferDesc);
		backBuffer->Release();
	}

	void Window::BeginFrame()
	{
		float background_color[] = { 0.25f, 0.5f, 1.f, 1.f };
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.ptr(), background_color);
	}

	void Window::EndFrame()
	{
		//Swapping buffers
		m_swapChain->Present(1, 0);
	}
}