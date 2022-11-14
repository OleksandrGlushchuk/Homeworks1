#pragma once
#include "Model.h"
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "../math/vec4.h"
#include "../math/Matr.h"
#include <memory>
#include "ConstantBuffer.h"
#include "singletones/TransformSystem.h"
#include "ModelID.h"
#include "MaterialConstantBuffer.h"

namespace engine
{
	class OpaqueInstances
	{
	public:
		struct Material
		{
			Texture m_colorMap;
			Texture m_normalMap;
			Texture m_roughnessMap;
			Texture m_metalnessMap;
			MaterialConstantBuffer m_materialConstantBuffer;

			Material(){}

			void Bind()
			{
				m_colorMap.Bind(0);
				m_normalMap.Bind(1);
				m_roughnessMap.Bind(2);
				m_metalnessMap.Bind(3);
			}

			bool operator==(const Material& mat) const
			{
				return m_colorMap == mat.m_colorMap;
			}
			bool operator!=(const Material& mat) const
			{
				return m_colorMap != mat.m_colorMap;
			}
		};

		struct Instance
		{
			uint32_t transform_id;
			uint16_t modelID;
			Instance() {}
			explicit Instance(const Transform& transform)
			{
				transform_id = engine::TransformSystem::instance().m_transforms.insert(transform);
			}
			explicit Instance(uint32_t copyTransform_ID) : transform_id(copyTransform_ID){}
		};

		struct GpuInstance
		{
			Matr<4> transform;
			uint16_t modelID;
			GpuInstance(){}
			explicit GpuInstance(const Instance& instance) : transform(TransformSystem::instance().m_transforms[instance.transform_id].getTransformMatrix()),
				modelID(instance.modelID)
			{}
		};

		struct MaterialInstance
		{
			MaterialInstance() {}
			MaterialInstance(const Material& _material, const Instance& _instance)
			{
				material = _material;
				instances.push_back(_instance);
			}
			Material material;
			std::vector<Instance> instances;
		};

		struct MeshInstance
		{
			MeshInstance() {}
			MeshInstance(const Material& material, const Instance& _instance)
			{
				materialInstances.emplace_back(material, _instance);
			}
			std::vector<MaterialInstance> materialInstances;
		};

		struct ModelInstance
		{
			ModelInstance() {}
			ModelInstance(const std::shared_ptr<Model>& _model, const std::vector<Material>& material, const Instance& _instance)
			{
				model = _model;
				meshInstances.resize(_model->m_meshes.size());

				for (uint32_t i = 0; i < _model->m_meshes.size(); ++i)
				{
					meshInstances[i] = MeshInstance(material[i], _instance);
				}

			}
			std::shared_ptr<Model> model;
			std::vector<MeshInstance> meshInstances;
			std::vector<uint32_t> meshIDs;
		};

		Shader m_shader;
		std::vector<ModelInstance> m_modelInstances;
		VertexBuffer<GpuInstance> m_instanceBuffer;
		ConstantBuffer<Matr<4>> m_constantBuffer;
		std::vector<ModelID> m_modelIDs;
		ConstantBuffer<MaterialConstantBuffer> m_materialConstantBuffer;
	public:
		OpaqueInstances() {}

		void Init();

		void updateInstanceBuffers();

		void render();

		void renderSceneDepthToCubemaps();

		void renderSceneDepthForDirectionalLights();
	};
}