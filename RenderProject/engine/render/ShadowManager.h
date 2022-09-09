#pragma once
#include "DxRes.hpp"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "Shader.h"
namespace engine
{
	class ShadowManager
	{
		static ShadowManager* s_instance;
		ShadowManager() {}
		ShadowManager(const ShadowManager& other) = delete;
		ShadowManager& operator=(const ShadowManager& other) = delete;
		ShadowManager(ShadowManager&& other) noexcept = delete;
		ShadowManager& operator=(ShadowManager&& other) noexcept = delete;

		struct PointLightIndex
		{
			PointLightIndex() {}
			PointLightIndex(uint32_t index) : pointLightIndex(index) {}
			uint32_t pointLightIndex;
			float padding[3];
		};
		D3D11_SHADER_RESOURCE_VIEW_DESC m_srvShadowDesc;
		RenderTarget m_shadowRenderTarget;
	public:
		static void init();
		static void deinit();
		static ShadowManager& instance();
		void UpdateShadowResources(UINT pointLightNum);
		void BindRenderTarget();

		DxResPtr<ID3D11ShaderResourceView> m_srvShadow;
		ConstantBuffer<PointLightIndex> m_pointLightIndex;
		Shader m_shadowShader;
	};
}