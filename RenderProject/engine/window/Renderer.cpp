#include "Renderer.h"

namespace engine::windows
{
	void Renderer::Init(UINT width, UINT height)
	{
		m_hdrRenderTarget.InitFloat16RenderTargetView(width, height);
		m_hdrRenderTarget.InitDepthStencil(width, height);

		ZeroMemory(&m_shaderResourceViewDesc, sizeof(m_shaderResourceViewDesc));
		m_shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_shaderResourceViewDesc.Texture2D.MipLevels = -1;
		m_shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		m_shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetRenderTergetResource(), &m_shaderResourceViewDesc, m_shaderResourceView.reset());
	}
	void Renderer::Draw()
	{
		m_sky.Draw();
	}

	void Renderer::Render(RenderTarget& windowRenderTarget, const Camera& camera, PostProcess& postProcess)
	{
		m_hdrRenderTarget.ClearRendetTargetView();
		m_hdrRenderTarget.ClearDepthStencil();

		m_hdrRenderTarget.Bind();

		engine::Globals::instance().UpdatePerFrameBuffer(camera);
		engine::Globals::instance().Bind();

		engine::MeshSystem::instance().render();
		m_sky.Draw();

		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

		postProcess.Resolve(windowRenderTarget, m_hdrRenderTarget);


		ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };
		engine::s_deviceContext->PSSetShaderResources(0, 1, SRVnullptr);
	}
}