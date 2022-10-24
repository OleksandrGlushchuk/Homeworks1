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
			m_renderTargetView.Release();
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
		m_renderTargetView.Init(m_backBuffer);
	}

	void Window::BeginFrame()
	{
		m_renderTargetView.Clear();
	}

	void Window::EndFrame()
	{
		//Swapping buffers
		m_swapChain1->Present(1, 0);
	}

	void Window::OnResize()
	{
		GetClientRect(m_wndHandle, &screen);
		initBackBuffer();
		m_renderTargetView.Init(m_backBuffer);
	}
}
