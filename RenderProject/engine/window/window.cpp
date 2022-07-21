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

	void Window::initDepthStencilResource()
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;

		depthBufferDesc.Width = std::max<UINT>(screen.right, 8u);
		depthBufferDesc.Height = std::max<UINT>(screen.bottom, 8u);
		depthBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT result = engine::s_device->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
	}

	void Window::initDepthStencilState()
	{

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;

		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP::D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;

		HRESULT result = engine::s_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilState");
	}

	void Window::initDepthStencilView()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		HRESULT result = engine::s_device->CreateDepthStencilView(m_depthStencilBuffer.ptr(), &depthStencilViewDesc, m_depthStencilView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilView");
	}

	void Window::initDepthStencilBuffer()
	{
		initDepthStencilResource();
		initDepthStencilState();
		initDepthStencilView();
	}

	void Window::BeginFrame()
	{
		engine::s_deviceContext->OMSetDepthStencilState(m_depthStencilState.ptr(), 1);
		engine::s_deviceContext->OMSetRenderTargets(1, (ID3D11RenderTargetView* const*)&m_renderTargetView1.ptr(), m_depthStencilView.ptr());

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, m_backbufferDesc.Width, m_backbufferDesc.Height);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		float background_color[] = { 0.25f, 0.5f, 1.f, 1.f };
		s_deviceContext->ClearRenderTargetView(m_renderTargetView1.ptr(), background_color);
		s_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 0.0f, 0);

	}

	void Window::EndFrame()
	{
		//Swapping buffers
		m_swapChain1->Present(1, 0);
	}

	void Window::OnResize()
	{
		GetClientRect(m_wndHandle, &screen);
		initDepthStencilResource();
		initDepthStencilView();
		initBackBuffer();
		initRenderTargetView();
	}
}
