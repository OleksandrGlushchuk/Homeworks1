#include "window.h"

//void Window::OnResize(uint32_t image_compression)
//{
//	GetClientRect(hwnd, &screen);
//	image.resize((screen.right/ image_compression) * (screen.bottom/ image_compression));
//	bitmap_info.bmiHeader.biWidth = screen.right/ image_compression;
//	bitmap_info.bmiHeader.biHeight = screen.bottom/ image_compression;
//}


namespace engine::windows
{
	void WindowD3D::initSwapchain()
	{
		DXGI_SWAP_CHAIN_DESC1 desc;

		// clear out the struct for use
		memset(&desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));

		// fill the swap chain description struct
		desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;                               // how many multisamples
		desc.SampleDesc.Quality = 0;                             // ???
		desc.Scaling = DXGI_SCALING_NONE;
		desc.Stereo = false;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		HRESULT res = s_factory->CreateSwapChainForHwnd(s_device, m_wndHandle, &desc, NULL, NULL, m_swapChain1.reset());
		ALWAYS_ASSERT(res >= 0 && "CreateSwapChainForHwnd");
	}

	void WindowD3D::initBackBuffer() // may be called after resizing
	{
		GetClientRect(m_wndHandle, &screen);
		if (m_backBuffer.valid())
		{
			m_renderTargetView1.release();
			m_backBuffer.release();
			HRESULT result = m_swapChain1->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			ALWAYS_ASSERT(result >= 0 && "ResizeBuffers");
		}

		HRESULT result = m_swapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_backBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "GetBuffer");

		m_backBuffer->GetDesc(&m_backbufferDesc);
	}

	void WindowD3D::initRenderTargetView()
	{
		HRESULT result = s_device->CreateRenderTargetView1(m_backBuffer, nullptr, m_renderTargetView1.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
	}

	void WindowD3D::BeginFrame()
	{
		engine::s_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)&m_renderTargetView1.ptr(), nullptr);

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, m_backbufferDesc.Width, m_backbufferDesc.Height);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		float background_color[] = { 0.25f, 0.5f, 1.f, 1.f };
		s_deviceContext->ClearRenderTargetView(m_renderTargetView1.ptr(), background_color);

	}

	void WindowD3D::EndFrame()
	{
		//Swapping buffers
		m_swapChain1->Present(1, 0);
	}

	void WindowD3D::OnResize()
	{
		initBackBuffer();
		initRenderTargetView();
	}
}
