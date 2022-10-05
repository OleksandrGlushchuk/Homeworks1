#include "Renderer.h"
#include "../render/singletones/ShadowManager.h"
#include "../render/singletones/ParticleSystem.h"

namespace engine::windows
{
	ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };
	void Renderer::Init(UINT sampleCount)
	{
		m_sampleCount = sampleCount;
		m_hdrRenderTarget.InitFloat16RTV(8, 8, sampleCount);
		m_hdrDepthStencil.Init(8, 8, sampleCount);

		ZeroMemory(&m_shaderResourceViewDesc, sizeof(m_shaderResourceViewDesc));
		m_shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (sampleCount == 1)
		{
			m_textureRegisterIndex = 0;
			m_shaderResourceViewDesc.Texture2D.MipLevels = -1;
			m_shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			m_shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			m_textureRegisterIndex = 1;
			m_shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetResourcePtr(), &m_shaderResourceViewDesc, m_shaderResourceView.reset());
		ParticleSystem::instance().CreateDepthSRVDesc(sampleCount);


		engine::s_device->CreateTexture2D(&m_hdrDepthStencil.GetResourceDesc(), nullptr , ParticleSystem::instance().m_depthTexture.reset());
		ParticleSystem::instance().CopyDepthTexture(m_hdrDepthStencil.GetDepthStencilResource());
		ParticleSystem::instance().CreateDepthSRV();
	}

	void Renderer::Render(RenderTargetView& windowRenderTarget, const Camera& camera, PostProcess& postProcess, float deltaTime)
	{
		if (need_to_resize_RTV)
		{
			ResizeRTV(windowRenderTarget);
			need_to_resize_RTV = false;
		}
		MeshSystem::instance().updateInstances();
		engine::Globals::instance().UpdatePerFrameBuffer(camera, m_sampleCount);
		engine::Globals::instance().Bind();

		RenderShadows();

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, windowRenderTarget.GetWidth(), windowRenderTarget.GetHeight());
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		m_hdrRenderTarget.Clear();
		m_hdrDepthStencil.Clear();
		m_hdrDepthStencil.BindDepthStencilState();
		engine::s_deviceContext->OMSetRenderTargets(1, m_hdrRenderTarget.GetRTVPtrToPrt(), m_hdrDepthStencil.GetDepthStencilViewPtr());

		engine::MeshSystem::instance().render();
		m_sky.BindEnvironment();
		m_sky.Draw();
		engine::MeshSystem::instance().grassField.render();
		
		
		ParticleSystem::instance().CopyDepthTexture(m_hdrDepthStencil.GetDepthStencilResource());
		ParticleSystem::instance().UpdateSmokeEmitters(camera, deltaTime);
		ParticleSystem::instance().render();

	


		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		engine::s_deviceContext->PSSetShaderResources(1 - m_textureRegisterIndex, 1, SRVnullptr);
		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, &m_shaderResourceView.ptr());
		postProcess.Resolve(windowRenderTarget);

		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, SRVnullptr);
	}

	void Renderer::ResizeRTV(RenderTargetView& windowRenderTarget)
	{
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();
		m_hdrRenderTarget.Resize(width, height);
		m_hdrDepthStencil.Resize(width, height);
		HRESULT result = engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetResourcePtr(), &m_shaderResourceViewDesc,
			m_shaderResourceView.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");

		result = engine::s_device->CreateTexture2D(&m_hdrDepthStencil.GetResourceDesc(), nullptr, ParticleSystem::instance().m_depthTexture.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateTexture2D");

		ParticleSystem::instance().CopyDepthTexture(m_hdrDepthStencil.GetDepthStencilResource());
		ParticleSystem::instance().CreateDepthSRV();
	}

	void Renderer::RenderShadows()
	{
		engine::ShadowManager::instance().BindPointLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthToCubemaps();

		engine::ShadowManager::instance().BindDirectionalLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthForDirectionalLights();
	}
}