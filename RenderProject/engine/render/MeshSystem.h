#pragma once
#include "ModelManager.h"
#include "OpaqueInstances.h"

namespace engine
{
	struct ModelID
	{
		uint32_t model_index;
		uint32_t meshesBlock_index;
	};
	class MeshSystem
	{
		OpaqueInstances opaqueInstances;
		static MeshSystem* s_instance;
		MeshSystem() {}
		MeshSystem(const MeshSystem& other) = delete;
		MeshSystem& operator=(const MeshSystem& other) = delete;
		MeshSystem(MeshSystem&& other) noexcept = delete;
		MeshSystem& operator=(MeshSystem&& other) noexcept = delete;
		void GetMaterialAndInstanceIndex(const ModelID& ID, uint32_t mesh_index, uint32_t& out_material_index, uint32_t& out_instance_index)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			out_material_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
			out_instance_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
		}
	public:
		static void init();
		static void deinit();
		static MeshSystem& instance();
		void addInstance(std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, ModelID& out_ID, const OpaqueInstances::Instance &_instance);
		
		void updateInstances()
		{
			opaqueInstances.updateInstanceBuffers();
		}

		void render()
		{
			updateInstances();
			opaqueInstances.render();
		}
		
		void Translate(const ModelID& ID, const Vec3& offset)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			uint32_t material_index, instance_index;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				GetMaterialAndInstanceIndex(ID, i, material_index, instance_index);
				auto& transform = mesh[i].materialInstances[material_index].instances[instance_index].m_transform;
				transform.position += offset;
				transform.UpdateMatrices();
			}
		}
		void SetPosition(const ModelID& ID, const Vec3& pos)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			uint32_t material_index, instance_index;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				GetMaterialAndInstanceIndex(ID, i, material_index, instance_index);
				auto& transform = mesh[i].materialInstances[material_index].instances[instance_index].m_transform;
				transform.position = pos;
				transform.UpdateMatrices();
			}
		}
		
		void Rotate(const ModelID& ID, const Quaternion& q)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			uint32_t material_index, instance_index;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				GetMaterialAndInstanceIndex(ID, i, material_index, instance_index);
				auto& transform = mesh[i].materialInstances[material_index].instances[instance_index].m_transform;
				transform.rotation *= q;
				transform.rotation.normalize();
				transform.UpdateMatrices();
			}
		}
		
		void Rotate(const ModelID& ID, const Angles& angles, Vec3 _right, Vec3 _top, Vec3 _forward)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			uint32_t material_index, instance_index;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				GetMaterialAndInstanceIndex(ID, i, material_index, instance_index);
				auto& transform = mesh[i].materialInstances[material_index].instances[instance_index].m_transform;
				transform.rotation *= Quaternion(angles.roll, _forward);
				transform.rotation *= Quaternion(angles.pitch, _right);
				transform.rotation *= Quaternion(angles.yaw, _top);
				transform.rotation.normalize();
				transform.UpdateMatrices();
			}
		}
		void Scale(const ModelID& ID, const Vec3& scale)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			uint32_t material_index, instance_index;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				GetMaterialAndInstanceIndex(ID, i, material_index, instance_index);
				auto& transform = mesh[i].materialInstances[material_index].instances[instance_index].m_transform;
				transform.scale *= scale;
				transform.UpdateMatrices();
			}
		}
		const Vec3& Get_Top(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.top();
		}
		const Vec3& Get_Forward(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.forward();
		}
		const Vec3& Get_Right(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.right();
		}
		const Vec3& Get_Position(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.position;
		}
		const Matr<4> Get_TransformMatrix(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.transform;
		}
		const Matr<4> Get_TransformInvMatrix(const ModelID& ID, uint32_t mesh_index = 0)
		{
			auto& model = opaqueInstances.m_modelInstances[ID.model_index];
			uint32_t material_index, instance_index;
			GetMaterialAndInstanceIndex(ID, mesh_index, material_index, instance_index);
			return model.meshInstances[mesh_index].materialInstances[material_index].instances[instance_index].m_transform.transformInv;
		}
	};
}