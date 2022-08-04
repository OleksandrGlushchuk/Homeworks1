#pragma once
#include "Model.h"
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "../math/vec4.h"
#include "../math/Matr.h"
#include <memory>
#include "ConstantBuffer.h"

class OpaqueInstances
{
public:
	struct Material
	{
		Texture m_texture;
		bool operator==(const Material& mat) const
		{
			return m_texture == mat.m_texture;
		}
		bool operator!=(const Material& mat) const
		{
			return m_texture != mat.m_texture;
		}
	};

	struct Instance
	{
		Matr<4> transform_matrix;
		Instance(){}
		explicit Instance(const Matr<4> &_transform_matrix) : transform_matrix(_transform_matrix){}
	};
	struct InstancePtr
	{
		Transform m_transform;
		InstancePtr() {}
		InstancePtr(const Instance &_instance)
		{
			m_transform = Transform(_instance.transform_matrix);
		}
		const Instance& Data() const
		{
			return Instance(m_transform.transform);
		}
	};

	struct MaterialInstance
	{
		MaterialInstance(){}
		MaterialInstance(const Material &_material, const Instance & _instance)
		{
			material = _material;
			instances.push_back(InstancePtr(_instance));
		}
		Material material;
		std::vector<InstancePtr> instances;
	};

	struct MeshInstance
	{
		MeshInstance(){}
		MeshInstance(const Material &material, const Instance& _instance)
		{
			materialInstances.emplace_back(material, _instance);
		}
		std::vector<MaterialInstance> materialInstances;
	};

	struct ModelInstance
	{
		ModelInstance(){}
		ModelInstance(std::shared_ptr<Model>& _model, const std::vector<Material>& material, const Instance& _instance)
		{
			model = _model;
			meshInstances.resize(_model->m_meshes.size());

			for (uint32_t i = 0; i < _model->m_meshes.size(); ++i)
			{
				meshInstances[i] = MeshInstance(material[i], _instance);
			}
			
		}
		bool empty() const { return meshInstances.size() == 0; }
		std::shared_ptr<Model> model;
		std::vector<MeshInstance> meshInstances;
		std::vector<uint32_t> meshIDs;
	};

	Shader m_shader;
	std::vector<ModelInstance> m_modelInstances;
	VertexBuffer<Instance> m_instanceBuffer;
	ConstantBuffer<Matr<4>> m_constantBuffer;
public:
	OpaqueInstances(){}

	void Init();

	void updateInstanceBuffers();

	void render();
};