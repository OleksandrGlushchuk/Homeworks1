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
#include "Model.h"
#include "ModelID.h"

namespace engine
{
	class EmissiveInstances
	{
	public:
		struct GpuInstance
		{
			Vec3 emission;
			float padding;
			Matr<4> transform_matrix;
			GpuInstance() {}
			GpuInstance(const Vec3& _emission, const Matr<4>& _transform_matrix) : emission(_emission), transform_matrix(_transform_matrix) {}
		};
		struct Instance
		{
			Vec3 emission;
			uint32_t transform_id;
			Instance(const Vec3& _emission, uint32_t _transform_id) : emission(_emission), transform_id(_transform_id) {}
			Instance(const Vec3& _emission, const Transform& transform)
			{
				transform_id = engine::TransformSystem::instance().m_transforms.insert(transform);
				emission = _emission;
			}
		};

		struct MeshInstance
		{
			MeshInstance() {}
			MeshInstance(const Instance& _instance)
			{
				instances.emplace_back(_instance);
			}
			std::vector<Instance> instances;
		};

		struct ModelInstance
		{
			ModelInstance() {}
			ModelInstance(const std::shared_ptr<Model>& _model, const Instance& _instance)
			{
				model = _model;
				meshInstances.resize(_model->m_meshes.size());

				for (uint32_t i = 0; i < _model->m_meshes.size(); ++i)
				{
					meshInstances[i] = MeshInstance(_instance);
				}

			}
			bool empty() const { return meshInstances.size() == 0; }
			std::shared_ptr<Model> model;
			std::vector<MeshInstance> meshInstances;
			std::vector<uint32_t> meshIDs;
		};

		Shader m_shader;
		std::vector<ModelInstance> m_modelInstances;
		VertexBuffer<GpuInstance> m_instanceBuffer;
		ConstantBuffer<Matr<4>> m_constantBuffer;
		std::vector<ModelID> m_modelIDs;
	public:
		EmissiveInstances() {}

		void Init();

		void updateInstanceBuffers();

		void render();
	};
}