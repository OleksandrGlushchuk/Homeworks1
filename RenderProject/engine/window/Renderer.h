#pragma once
#include "../objects/Sky.h"
#include "../render/DepthStencil.h"
#include "../render/PostProcess.h"

namespace engine::windows
{
	class Renderer
	{
		Sky m_sky;
		RenderTarget m_hdrRenderTarget;
		DxResPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc;
		uint32_t m_sampleCount;
		uint32_t m_textureRegisterIndex;
		bool need_to_resize_RTV;
	public:
		void Init(UINT sampleCount);
		void Render(RenderTarget& windowRenderTarget, const Camera& camera, PostProcess &postProcess);
		void ResizeRTV(RenderTarget& windowRenderTarget);
		friend class Application;
	};
}