#pragma once
#include "DxRes.hpp"
#include "ConstantBuffer.h"
#include "DepthStencil.h"
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

		struct LightIndexBuffer
		{
			LightIndexBuffer() {}
			LightIndexBuffer(uint32_t index) : lightIndex(index) {}
			uint32_t lightIndex;
			float padding[3];
		};
		struct ResolutionBuffer
		{
			ResolutionBuffer(){}
			ResolutionBuffer(float _resolution) : resolution(_resolution) {}
			float resolution;
			float padding[3];
		};
		D3D11_SHADER_RESOURCE_VIEW_DESC m_srvPointLightShadowDesc;
		DepthStencil m_pointLightShadowDepthStencil;

		D3D11_SHADER_RESOURCE_VIEW_DESC m_srvDirectionalLightShadowDesc;
		DepthStencil m_directionalLightShadowDepthStencil;

		float m_directionalLightDSResolution;
		float m_pointLightDSResolution;

	public:
		static void init();
		static void deinit();
		static ShadowManager& instance();
		void UpdatePointLightShadowResources(UINT pointLightNum);
		void UpdateDirectionalLightShadowResources(UINT pointLightNum);
		void BindPointLightDepthStencils();
		void BindDirectionalLightDepthStencils();
		float GetDirectionalLightDSResolution()
		{
			return m_directionalLightDSResolution;
		}
		float GetPointLightDSResolution()
		{
			return m_pointLightDSResolution;
		}
		void SetDirectionalLightDSResolution(float resolution)
		{
			m_directionalLightDSResolution = resolution;
		}
		void SetPointLightDSResolution(float resolution)
		{
			m_pointLightDSResolution = resolution;
			m_pointLightDSResolutionBuffer.Update(m_pointLightDSResolution);
		}

		DxResPtr<ID3D11ShaderResourceView> m_srvPointLightShadow;
		DxResPtr<ID3D11ShaderResourceView> m_srvDirectionalLightShadow;

		ConstantBuffer<LightIndexBuffer> m_lightIndexBuffer;
		ConstantBuffer<ResolutionBuffer> m_pointLightDSResolutionBuffer;
		Shader m_pointLightShadowShader;
		Shader m_directionalLightShadowShader;
	};
}