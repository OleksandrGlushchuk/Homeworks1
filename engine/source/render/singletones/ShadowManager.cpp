#include "ShadowManager.h"
#include "LightSystem.h"

namespace engine
{
	ShadowManager* ShadowManager::s_instance;
	void ShadowManager::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ShadowManager;
		s_instance->m_pointLightShadowBuffer.Init(D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		s_instance->m_directionalLightShadowBuffer.Init(D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		s_instance->m_pointLightDSResolutionBuffer.Init(D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

		s_instance->m_srvPointLightShadowDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		s_instance->m_srvPointLightShadowDesc.TextureCubeArray.First2DArrayFace = 0;
		s_instance->m_srvPointLightShadowDesc.TextureCubeArray.MipLevels = 1;
		s_instance->m_srvPointLightShadowDesc.TextureCubeArray.MostDetailedMip = 0;
		s_instance->m_srvPointLightShadowDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;

		s_instance->m_srvDirectionalLightShadowDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		s_instance->m_srvDirectionalLightShadowDesc.Texture2DArray.FirstArraySlice = 0;
		s_instance->m_srvDirectionalLightShadowDesc.Texture2DArray.MipLevels = 1;
		s_instance->m_srvDirectionalLightShadowDesc.Texture2DArray.MostDetailedMip = 0;
		s_instance->m_srvDirectionalLightShadowDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
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
	void ShadowManager::UpdatePointLightShadowResources(UINT pointLightNum)
	{
		if (pointLightNum == 0)
			return;
		m_pointLightShadowDepthStencil.InitCubeMapArray(m_pointLightDSResolution, pointLightNum);
		m_srvPointLightShadowDesc.TextureCubeArray.NumCubes = pointLightNum;
		HRESULT result = s_device->CreateShaderResourceView(m_pointLightShadowDepthStencil.GetDepthStencilResource(), 
			&m_srvPointLightShadowDesc, m_srvPointLightShadow.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");
	}
	void ShadowManager::UpdateDirectionalLightShadowResources(UINT directionalLightNum)
	{
		if (directionalLightNum == 0)
			return;
		m_directionalLightShadowDepthStencil.InitTexture2DArray(m_directionalLightDSResolution, directionalLightNum);
		m_srvDirectionalLightShadowDesc.Texture2DArray.ArraySize = directionalLightNum;
		HRESULT result = s_device->CreateShaderResourceView(m_directionalLightShadowDepthStencil.GetDepthStencilResource(),
			&m_srvDirectionalLightShadowDesc, m_srvDirectionalLightShadow.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");
	}
	void ShadowManager::BindPointLightDepthStencils()
	{
		if (LightSystem::instance().getPointLights().size() == 0)
			return;
		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, m_pointLightDSResolution, m_pointLightDSResolution);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		m_pointLightShadowDepthStencil.Clear();
		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, m_pointLightShadowDepthStencil.GetDepthStencilView().ptr());
	}

	void ShadowManager::BindDirectionalLightDepthStencils()
	{
		if (LightSystem::instance().getDirectionalLights().size() == 0)
			return;
		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, m_directionalLightDSResolution, m_directionalLightDSResolution);
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		m_directionalLightShadowDepthStencil.Clear();
		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, m_directionalLightShadowDepthStencil.GetDepthStencilView().ptr());
	}

	ShadowManager::PointLightShadowBuffer::PointLightShadowBuffer(uint32_t _lightIndex) : lightIndex(_lightIndex)
	{
		const auto& pl_matrices = LightSystem::instance().getAllViewProjPointLight();
		std::copy(pl_matrices.begin() + _lightIndex * 6, pl_matrices.begin() + (_lightIndex + 1) * 6, viewProjPointLight);
	}

	ShadowManager::DirectionalLightShadowBuffer::DirectionalLightShadowBuffer(uint32_t _lightIndex) : lightIndex(_lightIndex)
	{
		viewProjDirectionalLight = LightSystem::instance().getViewProjDirectionalLight(_lightIndex);
	}
}
