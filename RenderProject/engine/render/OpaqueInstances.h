#pragma once
#include "Model.h"
#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "../math/vec4.h"
#include "../math/Matr.h"
#include <memory>

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
		Vec4 mesh_to_model_x;
		Vec4 mesh_to_model_y;
		Vec4 mesh_to_model_z;
		Vec4 mesh_to_model_w;

		Vec4 transform_x;
		Vec4 transform_y;
		Vec4 transform_z;
		Vec4 transform_w;
	};
	struct InstancePtr
	{
		Matr<4> meshToModelMatrix;
		Transform m_transform;
		InstancePtr() {}
		InstancePtr(Matr<4> &_meshToModelMatrix)
		{
			meshToModelMatrix = _meshToModelMatrix;
		}
		const Instance& Data() const
		{
			Instance ret;

			ret.mesh_to_model_x = meshToModelMatrix[0];
			ret.mesh_to_model_y = meshToModelMatrix[1];
			ret.mesh_to_model_z = meshToModelMatrix[2];
			ret.mesh_to_model_w = meshToModelMatrix[3];

			ret.transform_x = m_transform.transform[0];
			ret.transform_y = m_transform.transform[1];
			ret.transform_z = m_transform.transform[2];
			ret.transform_w = m_transform.transform[3];

			return ret;
		}
	};

	struct MaterialInstance
	{
		MaterialInstance(){}
		MaterialInstance(const Material &_material, Matr<4> &meshToModelMatrix)
		{
			material = _material;
			instances.push_back(InstancePtr(meshToModelMatrix));
		}
		Material material;
		std::vector<InstancePtr> instances;
	};

	struct MeshInstance
	{
		MeshInstance(){}
		MeshInstance(const Material &material, Matr<4> &meshToModelMatrix)
		{
			materialInstances.emplace_back(material, meshToModelMatrix);
		}
		std::vector<MaterialInstance> materialInstances;
	};

	struct ModelInstance
	{
		ModelInstance(){}
		ModelInstance(std::shared_ptr<Model>& _model, const std::vector<Material>& material)
		{
			model = _model;
			meshInstances.resize(_model->m_meshes.size());

			for (uint32_t i = 0; i < _model->m_meshes.size(); ++i)
			{
				meshInstances[i] = MeshInstance(material[i], _model->m_meshes[i].meshToModelMatrix);
			}
			
		}
		bool empty() const { return meshInstances.size() == 0; }
		std::shared_ptr<Model> model;
		std::vector<MeshInstance> meshInstances;
	};

	Shader m_shader;
	std::vector<ModelInstance> m_modelInstances;
	VertexBuffer<Instance> m_instanceBuffer;

public:
	OpaqueInstances(){}

	void Init();

	void updateInstanceBuffers();

	void render();
};