#include "Renderer.h"

namespace engine::windows
{
	void Renderer::Init(UINT width, UINT height)
	{
		m_hdrRenderTarget.InitFloat16RenderTargetView(width, height);
		m_hdrRenderTarget.InitDepthStencil(width, height);
		
	}
	void Renderer::Draw()
	{
		m_sky.Draw();
	}

	void Renderer::Render(RenderTarget& windowRenderTarget, const Camera& camera, PostProcess& postProcess)
	{
		UINT width = windowRenderTarget.GetWidth();
		UINT height = windowRenderTarget.GetHeight();

		m_hdrRenderTarget.ResizeRenderTargetView(width, height);
		m_hdrRenderTarget.ClearRendetTargetView();

		m_hdrRenderTarget.ResizeDepthStencil(width, height);
		m_hdrRenderTarget.ClearDepthStencil();

		m_hdrRenderTarget.Bind();

		engine::Globals::instance().UpdatePerFrameBuffer(camera);
		engine::Globals::instance().Bind();

		engine::MeshSystem::instance().render();
		m_sky.Draw();

		engine::s_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.Texture2D.MipLevels = -1;
		desc.Texture2D.MostDetailedMip = 0;
		desc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

		engine::s_device->CreateShaderResourceView(m_hdrRenderTarget.GetRenderTergetResource(), &desc, m_shaderResourceView.reset());
		engine::s_deviceContext->PSSetShaderResources(0, 1, &m_shaderResourceView.ptr());

		postProcess.Resolve(windowRenderTarget, m_hdrRenderTarget);
	}
}