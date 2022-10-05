#pragma once
#include "../DxRes.hpp"
#include "../VertexBuffer.h"
#include "../SmokeEmitter.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../Model.h"
#include "../ConstantBuffer.h"
#include <memory>
class Camera;
namespace engine
{
	class ParticleSystem
	{
		struct ParticleConstantBuffer
		{
			ParticleConstantBuffer(){}
			ParticleConstantBuffer(uint32_t _colsNum, uint32_t _rowsNum) : colsNum(_colsNum), rowsNum(_rowsNum){}
			uint32_t colsNum;
			uint32_t rowsNum;
			float paddingAS[2];
		};

		static ParticleSystem* s_instance;
		ParticleSystem() {}
		ParticleSystem(const ParticleSystem& other) = delete;
		ParticleSystem& operator=(const ParticleSystem& other) = delete;
		ParticleSystem(ParticleSystem&& other) noexcept = delete;
		ParticleSystem& operator=(ParticleSystem&& other) noexcept = delete;

		DxResPtr<ID3D11BlendState> m_blendState;
		DxResPtr<ID3D11DepthStencilState> m_readonlyDepthStencilState;

		Texture m_smokeEMVA;
		Texture m_smokeRLT;
		Texture m_smokeBotBF;

		DxResPtr<ID3D11ShaderResourceView> m_depthSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC m_depthSRVDesc;

		ConstantBuffer<ParticleConstantBuffer> m_particleConstantBuffer;

		Shader m_shader;
		std::shared_ptr<Model> m_particleModel;
		VertexBuffer<GpuParticle> m_particleInstanceBuffer;

		std::vector<SmokeEmitter> m_smokeEmitters;
		uint32_t m_depthTextureRegisterIndex;
	public:
		static void init();
		static void deinit();
		static ParticleSystem& instance();
		DxResPtr<ID3D11Texture2D> m_depthTexture;
		void render();
		void CreateDepthSRVDesc(uint32_t sampleCount);
		void CreateDepthSRV();
		void CopyDepthTexture(ID3D11Texture2D* src);
		void AddSmokeEmitter(const SmokeEmitter& smokeEmitter) { m_smokeEmitters.emplace_back(smokeEmitter); }
		void UpdateSmokeEmitters(const Camera& camera, float deltaTime);
	};
}