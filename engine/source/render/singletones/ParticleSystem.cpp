#include "ParticleSystem.h"
#include "BlendStateManager.h"
#include "DepthStencilManager.h"
#include "ModelManager.h"
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
		engine::s_deviceContext->PSSetShaderResources(m_depthTextureRegisterIndex + 3, 1, &m_depthSRV.ptr());

		if (m_particleInstanceBuffer.Size() == 0)
			return;

		m_smokeEMVA.Bind(0);
		m_smokeRLT.Bind(1);
		m_smokeBotBF.Bind(2);

		m_shader.Bind();
		m_particleConstantBuffer.BindVS(1);

		m_particleModel->m_vertexBuffer.Bind(0);
		m_particleInstanceBuffer.Bind(1);

		s_deviceContext->DrawIndexedInstanced(m_particleModel->m_meshes[0].indexNum, m_particleInstanceBuffer.Size(), 0, 0, 0);

		engine::s_deviceContext->OMSetBlendState(nullptr, nullptr, sampleMask);

		engine::s_deviceContext->PSSetShaderResources(m_depthTextureRegisterIndex + 3, 1, SRVnullptr);
	}
	
	void ParticleSystem::CreateDepthSRVDesc(uint32_t sampleCount)
	{
		if (sampleCount == 1)
		{
			s_instance->m_depthSRVDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, 0, 1);
			m_depthTextureRegisterIndex = 0;
		}
		else //DELETE
		{
			s_instance->m_depthSRVDesc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2DMS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS);
			m_depthTextureRegisterIndex = 1;
		}
	}

	void ParticleSystem::CreateDepthSRV()
	{
		HRESULT result = engine::s_device->CreateShaderResourceView(m_depthTexture.ptr(), &m_depthSRVDesc, m_depthSRV.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateShaderResourceView");
	}
	void ParticleSystem::CopyDepthTexture(ID3D11Texture2D* src)
	{
		engine::s_deviceContext->CopyResource(m_depthTexture.ptr(), src);
	}
}
