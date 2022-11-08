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

		{
			D3D11_BUFFER_DESC bdesc;
			bdesc.ByteWidth = sizeof(ParticleData) * 128;
			bdesc.Usage = D3D11_USAGE_DEFAULT;
			bdesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			bdesc.CPUAccessFlags = 0;
			bdesc.StructureByteStride = sizeof(ParticleData);
			bdesc.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			HRESULT result = s_device->CreateBuffer(&bdesc, nullptr, s_instance->m_particlesData.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
		}

		{
			D3D11_BUFFER_DESC bdesc;
			bdesc.ByteWidth = sizeof(uint32_t) * 3;
			bdesc.Usage = D3D11_USAGE_DEFAULT;
			bdesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			bdesc.CPUAccessFlags = 0;
			bdesc.StructureByteStride = 0;
			bdesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subresource = { 0 };
			uint32_t data[3]{ 0,0,0 };
			subresource.pSysMem = data;

			HRESULT result = s_device->CreateBuffer(&bdesc, &subresource, s_instance->m_particlesRange.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
		}
		
		{
			D3D11_BUFFER_DESC bdesc;
			bdesc.ByteWidth = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
			bdesc.Usage = D3D11_USAGE_DEFAULT;
			bdesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
			bdesc.CPUAccessFlags = 0;
			bdesc.StructureByteStride = sizeof(D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS);
			bdesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS | D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			D3D11_SUBRESOURCE_DATA subresource = { 0 };
			D3D11_DRAW_INDEXED_INSTANCED_INDIRECT_ARGS data = { 0 };
			subresource.pSysMem = &data;

			HRESULT result = s_device->CreateBuffer(&bdesc, &subresource, s_instance->m_particlesIndirectArgs.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
		}

		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
			udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			udesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			udesc.Buffer.FirstElement = 0;
			udesc.Buffer.NumElements = 128;
			udesc.Buffer.Flags = 0;//D3D11_BUFFER_UAV_FLAG::D3D11_BUFFER_UAV_FLAG_COUNTER;
			HRESULT result = s_device->CreateUnorderedAccessView(s_instance->m_particlesData.ptr(), &udesc, s_instance->m_particlesDataUAV.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateUnorderedAccessView");
		}

		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
			udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			udesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
			udesc.Buffer.FirstElement = 0;
			udesc.Buffer.NumElements = 3;
			udesc.Buffer.Flags = 0;
			HRESULT result = s_device->CreateUnorderedAccessView(s_instance->m_particlesRange.ptr(), &udesc, s_instance->m_particlesRangeUAV.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateUnorderedAccessView");
		}

		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
			udesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			udesc.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
			udesc.Buffer.FirstElement = 0;
			udesc.Buffer.NumElements = 1;
			udesc.Buffer.Flags = 0;//D3D11_BUFFER_UAV_FLAG::D3D11_BUFFER_UAV_FLAG_COUNTER;
			HRESULT result = s_device->CreateUnorderedAccessView(s_instance->m_particlesIndirectArgs.ptr(), &udesc, s_instance->m_particlesIndirectArgsUAV.reset());
			ALWAYS_ASSERT(result >= 0 && "CreateUnorderedAccessView");
		}

		s_instance->m_sparks_spawning_shader.Init(L"engine/shaders/sparks_spawning.hlsl", MeshSystem::instance().incinerationInstances.m_inputDesc,12,ShaderEnabling(0,0));
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
		ID3D11ShaderResourceView* SRVnullptr[1] = { nullptr };
		engine::s_deviceContext->OMSetBlendState(m_blendState.ptr(), blendFactor, sampleMask);
		engine::s_deviceContext->OMSetDepthStencilState(m_readonlyDepthStencilState.ptr(), 1);

		engine::s_deviceContext->PSSetShaderResources(1 - m_depthTextureRegisterIndex + 3, 1, SRVnullptr);
		m_depthTexture.Bind(m_depthTextureRegisterIndex + 3);

		if (m_particleInstanceBuffer.Size() == 0)
			return;

		m_smokeEMVA.Bind(0);
		m_smokeRLT.Bind(1);
		m_smokeBotBF.Bind(2);

		m_shader.Bind();
		m_particleConstantBuffer.BindVS(1);

		m_particleModel->m_vertexBuffer.Bind(0);
		m_particleModel->m_indexBuffer.Bind();

		m_particleInstanceBuffer.Bind(1);

		s_deviceContext->DrawIndexedInstanced(m_particleModel->m_meshes[0].indexNum, m_particleInstanceBuffer.Size(), 0, 0, 0);

		engine::s_deviceContext->OMSetBlendState(nullptr, nullptr, sampleMask);

		engine::s_deviceContext->PSSetShaderResources(m_depthTextureRegisterIndex + 3, 1, SRVnullptr);

		spawnSparks();
		updateSparks();
		updateSparksRange();
		renderSparks();
	}


	void ParticleSystem::spawnSparks()
	{
		m_sparks_spawning_shader.Bind();
		ID3D11UnorderedAccessView* uavs[2]{ m_particlesDataUAV.ptr(), m_particlesRangeUAV.ptr() };
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 2, uavs, nullptr);

		MeshSystem::instance().incinerationInstances.renderForParticleSystem();

		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
	}

	void ParticleSystem::updateSparks()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_particlesDataUAV.ptr(), m_particlesRangeUAV.ptr() };
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 2, uavs, nullptr);

		m_sparks_updation_shader.BindCompute();
		s_deviceContext->Dispatch(2, 1, 1);

		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
	}

	void ParticleSystem::updateSparksRange()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_particlesIndirectArgsUAV.ptr(), m_particlesRangeUAV.ptr() };
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 2, uavs, nullptr);

		m_sparks_range_updation_shader.BindCompute();
		s_deviceContext->Dispatch(1, 1, 1);

		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
	}

	void ParticleSystem::renderSparks()
	{
		ID3D11UnorderedAccessView* uavs[2]{ m_particlesDataUAV.ptr(), m_particlesRangeUAV.ptr() };
		m_sparkColor.Bind(0);
		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, 2,
			uavs, nullptr);

		s_deviceContext->DrawInstancedIndirect(m_particlesIndirectArgs, 0);

		engine::s_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
	}
}
