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
#include "OpaqueInstances.h"
#include "MaterialConstantBuffer.h"

namespace engine
{
	class DissolubleInstances
	{
	public:
		struct Instance
		{
			uint32_t transform_id;
			float creationTime;
			float lifeTime;
			Instance() {}
			explicit Instance(const Transform& transform, float _creationTime, float _lifeTime) : creationTime(_creationTime), lifeTime(_lifeTime),
				transform_id(engine::TransformSystem::instance().m_transforms.insert(transform))
			{}

			explicit Instance(uint32_t copyTransform_ID, float _creationTime, float _lifeTime) : 
				creationTime(_creationTime), lifeTime(_lifeTime), transform_id(copyTransform_ID) {}
		};

		struct GpuInstance
		{
			Matr<4> transformMatrix;
			float creationTime;
			float lifeTime;

			GpuInstance(){}
			GpuInstance(const Instance& instance) : creationTime(instance.creationTime), lifeTime(instance.lifeTime),
				transformMatrix(engine::TransformSystem::instance().m_transforms[instance.transform_id].getTransformMatrix())
			{}
		};

		struct MaterialInstance
		{
			MaterialInstance() {}
			MaterialInstance(const OpaqueInstances::Material& _material, const Instance& _instance)
			{
				material = _material;
				instances.push_back(_instance);
			}
			OpaqueInstances::Material material;
			std::vector<Instance> instances;
		};

		struct MeshInstance
		{
			MeshInstance() {}
			MeshInstance(const OpaqueInstances::Material& material, const Instance& _instance)
			{
				materialInstances.emplace_back(material, _instance);
			}
			std::vector<MaterialInstance> materialInstances;
		};

		struct ModelInstance
		{
			ModelInstance() {}
			ModelInstance(const std::shared_ptr<Model>& _model, const std::vector<OpaqueInstances::Material>& material, const Instance& _instance)
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
		Texture m_dissolubleMap;
		Texture m_dissolubleMap1;
		DxResPtr<ID3D11BlendState> m_blendState;
	public:
		DissolubleInstances() {}

		void Init();

		void updateInstanceBuffers();

		void render();

		void renderSceneDepthToCubemaps();

		void renderSceneDepthForDirectionalLights();
	};
}