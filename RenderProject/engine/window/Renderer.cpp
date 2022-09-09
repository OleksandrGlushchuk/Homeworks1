#include "Renderer.h"

namespace engine::windows
{
	void Renderer::Init(UINT sampleCount)
	{
		m_sampleCount = sampleCount;
		m_hdrRenderTarget.InitFloat16RenderTargetView(8, 8, sampleCount);
		m_hdrRenderTarget.InitDepthStencil(8, 8, sampleCount);

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
		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetRenderTergetResource(), &m_shaderResourceViewDesc, m_shaderResourceView.reset());
	}

	void Renderer::Render(RenderTarget& windowRenderTarget, const Camera& camera, PostProcess& postProcess)
	{
		if (need_to_resize_RTV)
		{
			ResizeRTV(windowRenderTarget);
			need_to_resize_RTV = false;
		}
		MeshSystem::instance().updateInstances();
		engine::Globals::instance().UpdatePerFrameBuffer(camera);
		engine::Globals::instance().Bind();
		engine::MeshSystem::instance().renderSceneDepthToCubemaps();

		auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, windowRenderTarget.GetWidth(), windowRenderTarget.GetHeight());
		engine::s_deviceContext->RSSetViewports(1, &viewPort);

		m_hdrRenderTarget.ClearRendetTargetView();
		m_hdrRenderTarget.ClearDepthStencil();
		m_hdrRenderTarget.Bind();
		m_sky.BindEnvironment();
		engine::MeshSystem::instance().render();
		m_sky.Draw();

		ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };

		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		engine::s_deviceContext->PSSetShaderResources(1 - m_textureRegisterIndex, 1, SRVnullptr);
		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, &m_shaderResourceView.ptr());

		postProcess.Resolve(windowRenderTarget, m_hdrRenderTarget, m_sampleCount);
		engine::s_deviceContext->PSSetShaderResources(m_textureRegisterIndex, 1, SRVnullptr);
	}

	void Renderer::ResizeRTV(RenderTarget& windowRenderTarget)
	{
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();
		m_hdrRenderTarget.ResizeRenderTargetView(width, height);
		m_hdrRenderTarget.ResizeDepthStencil(width, height);
		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetRenderTergetResource(), &m_shaderResourceViewDesc,
			m_shaderResourceView.reset());
	}
}