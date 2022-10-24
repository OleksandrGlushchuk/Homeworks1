#pragma once
#include "../DxRes.hpp"
#include "../ConstantBuffer.h"
#include "../DepthStencil.h"
#include "../Shader.h"
#include "../../math/Matr.h"
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

		
		D3D11_SHADER_RESOURCE_VIEW_DESC m_srvPointLightShadowDesc;
		DepthStencil m_pointLightShadowDepthStencil;

		D3D11_SHADER_RESOURCE_VIEW_DESC m_srvDirectionalLightShadowDesc;
		DepthStencil m_directionalLightShadowDepthStencil;

		float m_directionalLightDSResolution;
		float m_directionalLightShadowDistance;
		float m_pointLightDSResolution;
	public:
		struct PointLightShadowBuffer
		{
			PointLightShadowBuffer() {}
			explicit PointLightShadowBuffer(uint32_t _lightIndex);

			uint32_t lightIndex;
			float padding[3];
			Matr<4> viewProjPointLight[6];
		};
		struct DirectionalLightShadowBuffer
		{
			DirectionalLightShadowBuffer() {}
			explicit DirectionalLightShadowBuffer(uint32_t _lightIndex);

			uint32_t lightIndex;
			float padding[3];
			Matr<4> viewProjDirectionalLight;
		};
		struct ResolutionBuffer
		{
			ResolutionBuffer() {}
			ResolutionBuffer(float _resolution) : resolution(_resolution) {}
			float resolution;
			float padding[3];
		};

		DxResPtr<ID3D11ShaderResourceView> m_srvPointLightShadow;
		DxResPtr<ID3D11ShaderResourceView> m_srvDirectionalLightShadow;
		ConstantBuffer<DirectionalLightShadowBuffer> m_directionalLightShadowBuffer;

		ConstantBuffer<PointLightShadowBuffer> m_pointLightShadowBuffer;
		ConstantBuffer<ResolutionBuffer> m_pointLightDSResolutionBuffer;

		static void init();
		static void deinit();
		static ShadowManager& instance();
		void UpdatePointLightShadowResources(UINT pointLightNum);
		void UpdateDirectionalLightShadowResources(UINT pointLightNum);
		void BindPointLightDepthStencils();
		void BindDirectionalLightDepthStencils();
		void SetDirectionalLightShadowDistance(float shadowDistance)
		{
			m_directionalLightShadowDistance = shadowDistance;
		}
		float GetDirectionalLightShadowDistance()
		{
			return m_directionalLightShadowDistance;
		}
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

		Shader m_pointLightOpaqueShadowShader;
		Shader m_directionalLightOpaqueShadowShader;

		Shader m_pointLightDissolubleShadowShader;
		Shader m_directionalLightDissolubleShadowShader;

		Shader m_pointLightGrassShadowShader;
		Shader m_directionalLightGrassShadowShader;
	};
}