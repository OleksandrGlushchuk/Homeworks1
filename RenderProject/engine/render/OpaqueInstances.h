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
		Vec4 transform_x;
		Vec4 transform_y;
		Vec4 transform_z;
		Vec4 transform_w;
	};
	struct InstancePtr
	{
		Transform m_transform;
		InstancePtr() {}
		InstancePtr(const Matr<4> &_transformMatrix)
		{
			m_transform = Transform(_transformMatrix);
		}
		const Instance& Data() const
		{
			Instance ret;
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
		MaterialInstance(const Material &_material, const Matr<4> & _transformMatrix)
		{
			material = _material;
			instances.push_back(InstancePtr(_transformMatrix));
		}
		Material material;
		std::vector<InstancePtr> instances;
	};

	struct MeshInstance
	{
		MeshInstance(){}
		MeshInstance(const Material &material, const Matr<4> & _transformMatrix)
		{
			materialInstances.emplace_back(material, _transformMatrix);
		}
		std::vector<MaterialInstance> materialInstances;
	};

	struct ModelInstance
	{
		ModelInstance(){}
		ModelInstance(std::shared_ptr<Model>& _model, const std::vector<Material>& material, const Matr<4>& _transformMatrix = Matr<4>::identity())
		{
			model = _model;
			meshInstances.resize(_model->m_meshes.size());

			for (uint32_t i = 0; i < _model->m_meshes.size(); ++i)
			{
				meshInstances[i] = MeshInstance(material[i], _transformMatrix);
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

public:
	OpaqueInstances(){}

	void Init();

	void updateInstanceBuffers();

	void render();
};