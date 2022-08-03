#include "window.h"

namespace engine::windows
{
	void Window::initSwapchain()
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

	void Window::initBackBuffer() // may be called after resizing
	{
		if (m_backBuffer.valid())
		{
			m_renderTargetView1.release();
			m_backBuffer.release();

			HRESULT result = m_swapChain1->ResizeBuffers(0, std::max<UINT>(screen.right,8), std::max<UINT>(screen.bottom, 8), DXGI_FORMAT_UNKNOWN, 0);
			ALWAYS_ASSERT(result >= 0 && "ResizeBuffers");
		}

		HRESULT result = m_swapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_backBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "GetBuffer");

		m_backBuffer->GetDesc(&m_backbufferDesc);
	}

	void Window::initRenderTargetView()
	{
		HRESULT result = s_device->CreateRenderTargetView1(m_backBuffer, nullptr, m_renderTargetView1.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateRenderTargetView1");
	}

	void Window::BeginFrame()
	{
		engine::s_deviceContext->OMSetDepthStencilState(engine::Globals::instance().m_depthStencilState.ptr(), 1);
		engine::s_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)&m_renderTargetView1.ptr(), engine::Globals::instance().m_depthStencilView.ptr());

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, m_backbufferDesc.Width, m_backbufferDesc.Height);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		float background_color[] = { 0.25f, 0.5f, 1.f, 1.f };
		s_deviceContext->ClearRenderTargetView(m_renderTargetView1.ptr(), background_color);
		s_deviceContext->ClearDepthStencilView(engine::Globals::instance().m_depthStencilView.ptr(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 0.0f, 0);

	}

	void Window::EndFrame()
	{
		//Swapping buffers
		m_swapChain1->Present(1, 0);
	}

	void Window::OnResize()
	{
		GetClientRect(m_wndHandle, &screen);
		engine::Globals::instance().UpdateDepthStencilBuffer(screen.right, screen.bottom);
		initBackBuffer();
		initRenderTargetView();
	}
}
