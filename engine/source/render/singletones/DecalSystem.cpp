#include "DecalSystem.h"
#include "ModelManager.h"
#include "RasteriserStateManager.h"
#include "TransformSystem.h"
#include <chrono>

namespace engine
{
	DecalSystem* DecalSystem::s_instance = nullptr;

	void DecalSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new DecalSystem;
		D3D11_INPUT_ELEMENT_DESC input[] =
		{
			{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"ALBEDO", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"MESH_ID", 0, DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"TRANSFORM_X", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_Y", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_Z", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_W", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"TRANSFORM_INV_X", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_INV_Y", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_INV_Z", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TRANSFORM_INV_W", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

		s_instance->m_shader.Init(L"engine/shaders/decal.hlsl", input, 11, ShaderEnabling(true, false));
		s_instance->m_decalModel = ModelManager::instance().GetUnitCubeModel();
		s_instance->m_random = std::uniform_real_distribution<float>(0.f, 1.f);
		s_instance->m_r_engine = std::default_random_engine(
			std::chrono::duration_cast<std::chrono::duration<uint32_t>>(std::chrono::steady_clock::now().time_since_epoch()).count());
		s_instance->m_normalMap.Load(L"engine/assets/DECAL.dds");
		s_instance->m_rasterizerState = RasteriserStateManager::instance().GetRasteriserState("back");
		s_instance->SetDecalSize(0.2f);
	}

	void DecalSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	DecalSystem& DecalSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	void DecalSystem::AddDecalInstance(const Vec3& right, const Vec3& up, const Vec3& forward, const Vec3 &pos, const Vec3 &normal, uint32_t meshTransformID, uint16_t meshID)
	{
		Decal& newDecal = m_instances.emplace_back();
		newDecal.meshTransformID = meshTransformID;
		newDecal.albedo = Vec3(m_random(m_r_engine), m_random(m_r_engine), m_random(m_r_engine));
		newDecal.meshID = meshID;
		float depth = MIN_DECAL_DEPTH + (MAX_DECAL_DEPTH - MIN_DECAL_DEPTH) * (1.f - Vec3::dot(normal, -forward));
		Matr<3> basis({
			{right.e[0] * m_decalSize, right.e[1] * m_decalSize, right.e[2] * m_decalSize},
			{up.e[0] * m_decalSize, up.e[1] * m_decalSize, up.e[2] * m_decalSize},
			{forward.e[0] * depth/2, forward.e[1] * depth/2, forward.e[2] * depth/2},
			});

		float angle = m_random(m_r_engine, std::uniform_real<float>::param_type(-M_PI, M_PI));
		float s = sinf(angle);
		float c = cosf(angle);
		Matr<3> rotation({
			{c, -s, 0},
			{s, c, 0},
			{0, 0, 1}
			});
		basis = rotation * basis;

		

		Matr<4> transform({
			{basis[0][0] , basis[0][1], basis[0][2] , 0},
			{basis[1][0] , basis[1][1] , basis[1][2] , 0},
			{basis[2][0], basis[2][1], basis[2][2], 0},
			{pos.e[0], pos.e[1], pos.e[2], 1}
			});

		newDecal.transformInMeshSpace = transform * TransformSystem::instance().m_transforms[meshTransformID].getTransformInvMatrix();
	}

	void DecalSystem::render()
	{
		if (m_instanceBuffer.Size() == 0)
			return;
		m_normalMap.Bind(0);
		m_decalModel->m_indexBuffer.Bind();
		m_decalModel->m_vertexBuffer.Bind(0);
		s_deviceContext->RSSetState(m_rasterizerState.ptr());
		m_instanceBuffer.Bind(1);
		m_shader.Bind();
		engine::s_deviceContext->DrawIndexedInstanced(m_decalModel->m_indexBuffer.Size(), m_instanceBuffer.Size(), 0, 0, 0);
		s_deviceContext->RSSetState(nullptr);
	}

	void DecalSystem::update()
	{
		uint32_t size = m_instances.size();
		if (size == 0)
			return;
		m_instanceBuffer.Init(size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
		auto mapping = m_instanceBuffer.Map();
		GpuDecal* dst = (GpuDecal*)mapping.pData;
		for (uint32_t i = 0; i < size; ++i)
		{
			dst[i] = GpuDecal(m_instances[i]);
		}
		m_instanceBuffer.Unmap();
	}
	DecalSystem::GpuDecal::GpuDecal(const Decal& decal) : albedo(decal.albedo), meshID(decal.meshID)
	{
		transform = decal.transformInMeshSpace * TransformSystem::instance().m_transforms[decal.meshTransformID].getTransformMatrix();
		transformInv = transform.invert();
	}
}