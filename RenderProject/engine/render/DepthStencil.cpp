#include "DepthStencil.h"

namespace engine
{
	void DepthStencil::initDepthStencilResource(UINT width, UINT height, UINT sampleCount)
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		depthBufferDesc.Width = std::max<UINT>(width, 8u);
		depthBufferDesc.Height = std::max<UINT>(height, 8u);
		depthBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.SampleDesc.Count = sampleCount;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT result = engine::s_device->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
	}
	void DepthStencil::initCubemapArrayDepthStencilResource(UINT size, UINT numCubemaps)
	{
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		depthBufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		depthBufferDesc.Width = std::max<UINT>(size, 8u);
		depthBufferDesc.Height = depthBufferDesc.Width;
		depthBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		depthBufferDesc.ArraySize = numCubemaps * 6;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
		HRESULT result = engine::s_device->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
	}

	void DepthStencil::initDepthStencilState()
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

	void DepthStencil::initDepthStencilView()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = m_sampleCount == 1 ? D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D :
			D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DMS;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		HRESULT result = engine::s_device->CreateDepthStencilView(m_depthStencilBuffer.ptr(), &depthStencilViewDesc, m_depthStencilView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilView");
	}

	void DepthStencil::initCubemapArrayDepthStencilView(UINT numCubemaps)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthStencilViewDesc.Texture2DArray.ArraySize = 6 * numCubemaps;
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
		depthStencilViewDesc.Texture2DArray.MipSlice = 0;

		HRESULT result = engine::s_device->CreateDepthStencilView(m_depthStencilBuffer.ptr(), &depthStencilViewDesc, m_depthStencilView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilView");
	}

	void DepthStencil::Init(UINT width, UINT height, UINT sampleCount)
	{
		m_sampleCount = sampleCount;
		initDepthStencilResource(width, height, sampleCount);
		initDepthStencilState();
		initDepthStencilView();
	}

	void DepthStencil::InitCubeMapArray(UINT size, UINT numCubemaps)
	{
		m_sampleCount = 1;
		initCubemapArrayDepthStencilResource(size, numCubemaps);
		initDepthStencilState();
		initCubemapArrayDepthStencilView(numCubemaps);
	}

	void DepthStencil::Resize(UINT width, UINT height)
	{
		initDepthStencilResource(width, height, m_sampleCount);
		initDepthStencilView();
	}

	void DepthStencil::BindDepthStencilState()
	{
		engine::s_deviceContext->OMSetDepthStencilState(m_depthStencilState.ptr(), 1);
	}

	void DepthStencil::Clear()
	{
		s_deviceContext->ClearDepthStencilView(m_depthStencilView.ptr(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 0.0f, 0); 
	}

}
