#include "Renderer.h"
#include "../render/ShadowManager.h"

namespace engine::windows
{
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
	}

	void Renderer::Render(RenderTargetView& windowRenderTarget, const Camera& camera, PostProcess& postProcess)
	{
		if (need_to_resize_RTV)
		{
			ResizeRTV(windowRenderTarget);
			need_to_resize_RTV = false;
		}
		MeshSystem::instance().updateInstances();
		engine::Globals::instance().UpdatePerFrameBuffer(camera);
		engine::Globals::instance().Bind();

		RenderShadows();

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, windowRenderTarget.GetWidth(), windowRenderTarget.GetHeight());
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		m_hdrRenderTarget.Clear();
		m_hdrDepthStencil.Clear();

		m_hdrDepthStencil.BindDepthStencilState();
		engine::s_deviceContext->OMSetRenderTargets(1, m_hdrRenderTarget.GetRTVPtrToPrt(), m_hdrDepthStencil.GetDepthStencilViewPtr());

		m_sky.BindEnvironment();
		engine::MeshSystem::instance().render();
		m_sky.Draw();

		ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };

		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		engine::s_deviceContext->PSSetShaderResources(1 - m_textureRegisterIndex, 1, SRVnullptr);
		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, &m_shaderResourceView.ptr());

		postProcess.Resolve(windowRenderTarget, m_sampleCount);

		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, SRVnullptr);
	}

	void Renderer::ResizeRTV(RenderTargetView& windowRenderTarget)
	{
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();
		m_hdrRenderTarget.Resize(width, height);
		m_hdrDepthStencil.Resize(width, height);
		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetResourcePtr(), &m_shaderResourceViewDesc,
			m_shaderResourceView.reset());
	}

	void Renderer::RenderShadows()
	{
		engine::ShadowManager::instance().BindPointLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthToCubemaps();

		engine::ShadowManager::instance().BindDirectionalLightDepthStencils();
		engine::MeshSystem::instance().renderSceneDepthForDirectionalLights();
	}
}