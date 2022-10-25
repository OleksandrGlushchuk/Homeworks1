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
		DepthStencil m_depthStencil;

		Texture m_screenTexture;
		uint32_t m_sampleCount;
		uint32_t m_textureRegisterIndex;
		bool need_to_resize_RTV;

		RenderTargetView m_normalsRTV;
		RenderTargetView m_albedoRTV;
		RenderTargetView m_rmfRTV;
		RenderTargetView m_emissionRTV;
		RenderTargetView m_modelIDRTV;

		Shader m_pl_deferredShadingShader;
		Shader m_dl_deferredShadingShader;
		Shader m_env_deferredShadingShader;

		Shader m_pl_grass_deferredShadingShader;
		Shader m_dl_grass_deferredShadingShader;
		Shader m_env_grass_deferredShadingShader;

		DxResPtr<ID3D11BlendState> m_additiveBlendState;
		DxResPtr<ID3D11DepthStencilState> m_defDepthStencilState;
		DxResPtr<ID3D11DepthStencilState> m_defDepthStencilWriteState;
		DxResPtr<ID3D11DepthStencilState> m_disDepthStencilReadState;
		DxResPtr<ID3D11DepthStencilState> m_disableDSS;

		Texture m_normalsTexture;
		Texture m_albedoTexture;
		Texture m_rmfTexture;
		Texture m_emissionTexture;
		Texture m_depthTexture;
		Texture m_copyNormalsTexture;
		Texture m_modelIDTexture;

		ID3D11RenderTargetView* m_GBuffer[5];

		void DeferredShading();
		void ForwardShading(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, float deltaTime);
		void Resolve(RenderTargetView& windowRenderTarget, PostProcess& postProcess);
		void RenderToGBuffer();
		void RenderShadows();
	public:
		void Init(UINT sampleCount);
		void Render(RenderTargetView& windowRenderTarget, const Camera& camera, PostProcess &postProcess,
			const std::chrono::steady_clock::time_point& currentTime, float deltaTime);
		void ResizeRTV(RenderTargetView& windowRenderTarget);
		friend class Application;
	};
}