#pragma once
#include "../objects/Sky.h"
#include "../render/DepthStencil.h"
#include "../render/RenderTargetView.h"
#include "../render/PostProcess.h"

namespace engine::windows
{
	class Renderer
	{
		Sky m_sky;
		RenderTargetView m_hdrRenderTarget;
		DepthStencil m_hdrDepthStencil;

		DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc;
		uint32_t m_sampleCount;
		uint32_t m_textureRegisterIndex;
		bool need_to_resize_RTV;
	public:
		void Init(UINT sampleCount);
		void Render(RenderTargetView& windowRenderTarget, const Camera& camera, PostProcess &postProcess,
			const std::chrono::steady_clock::time_point& currentTime, float deltaTime);
		void ResizeRTV(RenderTargetView& windowRenderTarget);
		void RenderShadows();
		friend class Application;
	};
}