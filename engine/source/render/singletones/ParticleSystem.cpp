#include "ParticleSystem.h"
#include "BlendStateManager.h"
#include "DepthStencilManager.h"
#include "ModelManager.h"
#include "LightSystem.h"
#include "../../math/camera.h"
static float blendFactor[4] = { 0.f,0.f,0.f,0.f };
static UINT sampleMask = 0xffffffff;
namespace engine
{
	ParticleSystem *ParticleSystem::s_instance;

	void ParticleSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new ParticleSystem;
		s_instance->m_blendState = BlendStateManager::instance().GetBlendState("default");
		s_instance->m_readonlyDepthStencilState = DepthStencilManager::instance().GetDepthStencilState("readonly");


		s_instance->m_smokeEMVA.Load(L"engine/assets/Smoke/test_EMVA_1.dds");
		s_instance->m_smokeRLT.Load(L"engine/assets/Smoke/test_RLT_1.dds");
		s_instance->m_smokeBotBF.Load(L"engine/assets/Smoke/test_BotBF_1.dds");

		s_instance->m_particleModel = ModelManager::instance().GetUnitQuadModel();
		ParticleConstantBuffer pcb(8, 8);
		s_instance->m_particleConstantBuffer.Init(D3D11_USAGE_DEFAULT, 0, &pcb);

		D3D11_INPUT_ELEMENT_DESC inputElem[] =
		{
			{"VERTEX_POS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"PARTICLE_POS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"PARTICLE_SIZE", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TINT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"ANGLE", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"THICKNESS", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"CREATION_TIME", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"LIFE_TIME", 0, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		s_instance->m_shader.Init(L"engine/shaders/particle.hlsl", inputElem, 8, ShaderEnabling(true, false));


		uint32_t data[3]{ 0,0,0 };
		D3D11_DRAW_INSTANCED_INDIRECT_ARGS subresource = { 0 };
		SparksBufferSize bufferSize(SPARKS_BUFFER_SIZE);
		s_instance->m_sparksData.Init(100'000, nullptr);
		s_instance->m_sparksRange.Init(3, DXGI_FORMAT_R32_UINT, data);
		s_instance->m_sparksIndirectArgs.Init();
		s_instance->m_sparksIndirectArgsWritable.Init(1, &subresource);
		s_instance->m_sparksBufferSize.Init(D3D11_USAGE_IMMUTABLE, 0, &bufferSize);

		s_instance->m_sparks_spawning_shader.Init(L"engine/shaders/sparks_spawning.hlsl", 
			MeshSystem::instance().incinerationInstances.m_inputDesc,12,ShaderEnabling(false, true, false));
		s_instance->m_sparks_updation_shader.InitCompute(L"engine/shaders/sparks_updation.hlsl");
		s_instance->m_sparks_range_updation_shader.InitCompute(L"engine/shaders/sparks_range_updation.hlsl");
		s_instance->m_sparks_drawing_shader.Init(L"engine/shaders/sparks_drawing.hlsl", nullptr, 0, ShaderEnabling(true, false));

		s_instance->m_sparkColor.Load(L"engine/assets/SPARK.dds");

	}
	void ParticleSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	ParticleSystem& ParticleSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void ParticleSystem::UpdateSmokeEmitters(const Camera& camera, const std::chrono::steady_clock::time_point& currentTime, float deltaTime)
	{
		if (deltaTime == 0) return;

		uint32_t particleNum = 0;
		for (auto& emitter : m_smokeEmitters)
		{
			emitter.Update(currentTime, deltaTime);
			particleNum += emitter.GetParticleNum();
		}

		if (particleNum != 0)
		{
			m_particleInstanceBuffer.Init(particleNum, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);

			auto mapping = m_particleInstanceBuffer.Map();
			GpuParticle* dst = static_cast<GpuParticle*>(mapping.pData);
			uint32_t copiedNum = 0;
			for (auto& emitter : m_smokeEmitters)
			{
				for (auto& particle : emitter.m_particles)
				{
					dst[copiedNum++] = GpuParticle(particle);
				}
			}
			std::sort(dst, dst + copiedNum, [&camera](GpuParticle& a, GpuParticle& b) {return (a.position - camera.position()).length() > (b.position - camera.position()).length(); });
			m_particleInstanceBuffer.Unmap();
		}
	}

	void ParticleSystem::render()
	{
		if (m_particleInstanceBuffer.Size() == 0)
			return;

		ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };
		engine::s_deviceContext->OMSetBlendState(m_blendState.ptr(), blendFactor, sampleMask);
		engine::s_deviceContext->OMSetDepthStencilState(m_readonlyDepthStencilState.ptr(), 1);

		engine::s_deviceContext->PSSetShaderResources(1 - m_depthTextureRegisterIndex + 3, 1, SRVnullptr);
		m_depthTexture.Bind(m_depthTextureRegisterIndex + 3);

		m_smokeEMVA.Bind(0);
		m_smokeRLT.Bind(1);
		m_smokeBotBF.Bind(2);

		m_shader.Bind();
		m_particleConstantBuffer.BindVS(1);

		m_particleModel->m_vertexBuffer.Bind(0);
		m_particleModel->m_indexBuffer.Bind();

		m_particleInstanceBuffer.Bind(1);

		s_deviceContext->DrawIndexedInstanced(m_particleModel->m_meshes[0].indexNum, m_particleInstanceBuffer.Size(), 0, 0, 0);

		engine::s_deviceContext->PSSetShaderResources(m_depthTextureRegisterIndex + 3, 1, SRVnullptr);
		engine::s_deviceContext->OMSetBlendState(nullptr, nullptr, sampleMask);
	}


	void ParticleSystem::spawnSparks()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_sparksData.m_uav.ptr(), m_sparksRange.m_uav.ptr() };
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 1, 2, uavs, nullptr);
		//engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_sparks_spawning_shader.Bind();
		MeshSystem::instance().incinerationInstances.renderForParticleSystem();
		uavs[0] = uavs[1] = nullptr;
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 1, 2, uavs, nullptr);
	}

	void ParticleSystem::updateSparks()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_sparksData.m_uav.ptr(), m_sparksRange.m_uav.ptr() };
		s_deviceContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);
		m_sparks_updation_shader.BindCompute();
		s_deviceContext->Dispatch(SPARKS_UPDATE_THREADX_NUM, 1, 1);
	}

	void ParticleSystem::updateSparksRange()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_sparksIndirectArgsWritable.m_uav.ptr(),  m_sparksRange.m_uav.ptr() };
		s_deviceContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);
		m_sparks_range_updation_shader.BindCompute();
		s_deviceContext->Dispatch(1, 1, 1);
		uavs[0] = uavs[1] = nullptr;
		s_deviceContext->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);
	}

	void ParticleSystem::renderSparks()
	{
		m_sparksBufferSize.BindCS(2);
		m_sparksBufferSize.BindVS(2);
		m_sparksBufferSize.BindGS(2);
		m_depthTexture.BindCS(0);

		spawnSparks();
		updateSparks();
		updateSparksRange();
		engine::s_deviceContext->OMSetDepthStencilState(m_readonlyDepthStencilState.ptr(), 1);
		engine::s_deviceContext->OMSetBlendState(m_blendState.ptr(), blendFactor, sampleMask);

		ID3D11UnorderedAccessView* uavs[2]{ m_sparksData.m_uav.ptr(), m_sparksRange.m_uav.ptr() };
		m_sparkColor.Bind(0);
		m_sparks_drawing_shader.Bind();
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 1, 2,
			uavs, nullptr);

		engine::s_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_deviceContext->CopyResource(m_sparksIndirectArgs.m_buffer.ptr(), m_sparksIndirectArgsWritable.m_buffer.ptr());
		s_deviceContext->DrawInstancedIndirect(m_sparksIndirectArgs.m_buffer.ptr(), 0);

		uavs[0] = uavs[1] = nullptr;
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 1, 2, uavs, nullptr);
		
		engine::s_deviceContext->OMSetBlendState(nullptr, nullptr, sampleMask);
	}
}
