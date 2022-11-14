#pragma once
#include "Texture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "MaterialConstantBuffer.h"
#include "Model.h"
#include "../math/vec3.h"

namespace engine
{
	class GrassField
	{
		struct GrassInstance
		{
			Vec3 position;

			GrassInstance(){}
			GrassInstance(const Vec3& _position) : position(_position) {}
		};
		Texture m_grassColor;
		Texture m_grassNormal;
		Texture m_grassRoughness;
		Texture m_grassMetalness; // is not used but binded
		Texture m_grassAO;
		Texture m_grassOpacity;
		Texture m_grassTranslucency;

		Shader m_shader;

		VertexBuffer<GrassInstance> m_instanceBuffer;
		ConstantBuffer<MaterialConstantBuffer> m_materialConstantBuffer;

		DxResPtr<ID3D11BlendState> m_blendState;
		DxResPtr<ID3D11RasterizerState> m_rasterizerState;
		std::vector<GrassInstance> m_instances;
		std::shared_ptr<Model> m_model;
	public:
		GrassField() {}
		void Init();
		void AddGrassArea(float width, float height, float r, const Vec3& world_pos);
		void render();
		void renderSceneDepthToCubemaps();
		void renderSceneDepthForDirectionalLights();
		uint32_t NumInstances()
		{
			return m_instances.size();
		}
	};
}