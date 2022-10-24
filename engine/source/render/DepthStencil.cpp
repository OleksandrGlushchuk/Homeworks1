#include "DepthStencil.h"
#include "singletones/DepthStencilManager.h"

namespace engine
{
	void DepthStencil::initDepthStencilResource(UINT width, UINT height, UINT sampleCount)
	{
		m_resourceDesc.Width = std::max<UINT>(width, 8u);
		m_resourceDesc.Height = std::max<UINT>(height, 8u);
		m_resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		m_resourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		m_resourceDesc.ArraySize = 1;
		m_resourceDesc.MipLevels = 1;
		m_resourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		m_resourceDesc.CPUAccessFlags = 0;
		m_resourceDesc.SampleDesc.Count = sampleCount;
		m_resourceDesc.SampleDesc.Quality = 0;
		m_resourceDesc.MiscFlags = 0;

		HRESULT result = engine::s_device->CreateTexture2D(&m_resourceDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");
	}
	void DepthStencil::initCubemapArrayDepthStencilResource(UINT size, UINT numCubemaps)
	{
		m_resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		m_resourceDesc.Width = std::max<UINT>(size, 8u);
		m_resourceDesc.Height = m_resourceDesc.Width;
		m_resourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		m_resourceDesc.ArraySize = numCubemaps * 6;
		m_resourceDesc.MipLevels = 1;
		m_resourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		m_resourceDesc.CPUAccessFlags = 0;
		m_resourceDesc.SampleDesc.Count = 1;
		m_resourceDesc.SampleDesc.Quality = 0;
		m_resourceDesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_TEXTURECUBE;
		HRESULT result = engine::s_device->CreateTexture2D(&m_resourceDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTextureCube");
	}

	void DepthStencil::initDepthStencilView()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (m_sampleCount == 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		else
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DMS;
		

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
		initDepthStencilView();
	}

	void DepthStencil::InitCubeMapArray(UINT size, UINT numCubemaps)
	{
		m_sampleCount = 1;
		initCubemapArrayDepthStencilResource(size, numCubemaps);
		initCubemapArrayDepthStencilView(numCubemaps);
	}

	void DepthStencil::initTexture2DArrayDepthStencilResource(UINT size, UINT numTextures)
	{
		m_resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		m_resourceDesc.Width = std::max<UINT>(size, 8u);
		m_resourceDesc.Height = m_resourceDesc.Width;
		m_resourceDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		m_resourceDesc.ArraySize = numTextures;
		m_resourceDesc.MipLevels = 1;
		m_resourceDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		m_resourceDesc.CPUAccessFlags = 0;
		m_resourceDesc.SampleDesc.Count = 1;
		m_resourceDesc.SampleDesc.Quality = 0;
		m_resourceDesc.MiscFlags = 0;
		HRESULT result = engine::s_device->CreateTexture2D(&m_resourceDesc, nullptr, m_depthStencilBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2DArray");
	}

	void DepthStencil::initTexture2DArrayDepthStencilView(UINT numTextures)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthStencilViewDesc.Texture2DArray.ArraySize = numTextures;
		depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
		depthStencilViewDesc.Texture2DArray.MipSlice = 0;

		HRESULT result = engine::s_device->CreateDepthStencilView(m_depthStencilBuffer.ptr(), &depthStencilViewDesc, m_depthStencilView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateDepthStencilView");
	}

	void DepthStencil::InitTexture2DArray(UINT size, UINT numTextures)
	{
		m_sampleCount = 1;
		initTexture2DArrayDepthStencilResource(size, numTextures);
		initTexture2DArrayDepthStencilView(numTextures);
	}

	void DepthStencil::Resize(UINT width, UINT height)
	{
		initDepthStencilResource(width, height, m_sampleCount);
		initDepthStencilView();
	}

	void DepthStencil::Clear()
	{
		s_deviceContext->ClearDepthStencilView(m_depthStencilView.ptr(), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 0.0f, 0);
	}

}
