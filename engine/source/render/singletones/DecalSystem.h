#pragma once
#include "../DxRes.hpp"
#include "../Texture.h"
#include "../Shader.h"
#include "../Model.h"
#include <random>

namespace engine
{
	class DecalSystem
	{
		struct Decal
		{
			Decal() {}
			uint32_t meshTransformID;
			Vec3 albedo;
			uint16_t meshID;
			Matr<4> transformInMeshSpace;
		};

		struct GpuDecal
		{
			Vec3 albedo;
			uint16_t meshID;
			Matr<4> transform;
			Matr<4> transformInv;
			GpuDecal() {}
			explicit GpuDecal(const Decal& decal);
		};

		static DecalSystem* s_instance;
		DecalSystem() {}
		DecalSystem(const DecalSystem& other) = delete;
		DecalSystem& operator=(const DecalSystem& other) = delete;
		DecalSystem(DecalSystem&& other) noexcept = delete;
		DecalSystem& operator=(DecalSystem&& other) noexcept = delete;
		Texture m_normalMap;
		Shader m_shader;
		std::vector<Decal> m_instances;
		VertexBuffer<GpuDecal> m_instanceBuffer;
		std::shared_ptr<Model> m_decalModel;
		std::default_random_engine m_r_engine;
		std::uniform_real_distribution<float> m_random;
		DxResPtr<ID3D11RasterizerState> m_rasterizerState;
		float m_decalSize;
		float MIN_DECAL_DEPTH;
		float MAX_DECAL_DEPTH;
	public:
		static void init();
		static void deinit();
		static DecalSystem& instance();
		void SetDecalSize(float newSize) { 
			m_decalSize = newSize/2;
			MIN_DECAL_DEPTH = m_decalSize/1.5f;
			MAX_DECAL_DEPTH = m_decalSize * 5;
		}
		void AddDecalInstance(const Vec3 &right, const Vec3& up, const Vec3 &forward, const Vec3& pos, const Vec3& normal, uint32_t meshTransformID, uint16_t meshID);
		void render();
		void update();
	};
}