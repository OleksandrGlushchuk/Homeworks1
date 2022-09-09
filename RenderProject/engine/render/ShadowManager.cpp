#include "ShadowManager.h"

namespace engine
{
	ShadowManager* ShadowManager::s_instance;
	void ShadowManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ShadowManager;
		s_instance->m_pointLightIndex.Init(D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

		s_instance->m_srvShadowDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		s_instance->m_srvShadowDesc.TextureCubeArray.First2DArrayFace = 0;
		s_instance->m_srvShadowDesc.TextureCubeArray.MipLevels = 1;
		s_instance->m_srvShadowDesc.TextureCubeArray.MostDetailedMip = 0;
		s_instance->m_srvShadowDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	}
	void ShadowManager::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}
	ShadowManager& ShadowManager::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}
	void ShadowManager::UpdateShadowResources(UINT pointLightNum)
	{
		m_shadowRenderTarget.InitCubeMapArrayDepthStencil(1024, pointLightNum);

		m_srvShadowDesc.TextureCubeArray.NumCubes = pointLightNum;
		HRESULT result = s_device->CreateShaderResourceView(m_shadowRenderTarget.GetDepthStencil().GetDepthStencilResource(), &m_srvShadowDesc, m_srvShadow.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");
	}
	void ShadowManager::BindRenderTarget()
	{
		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, 1024, 1024);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);
		m_shadowRenderTarget.ClearDepthStencil();
		m_shadowRenderTarget.Bind();
	}
}
