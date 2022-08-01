#pragma once
#include "ModelManager.h"
#include "OpaqueInstances.h"

namespace engine
{
	struct MeshID
	{
		uint32_t material_index;
		uint32_t instance_index;
	};
	struct ObjectID
	{
		uint32_t model_index;
		std::vector<MeshID> meshID;
	};
	class MeshSystem
	{
		OpaqueInstances opaqueInstances;
		static MeshSystem* s_instance;
		MeshSystem() {}
	public:
		static void init();
		static void deinit();
		static MeshSystem& instance();
		void addInstance(std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, ObjectID& out_ID);

		void render()
		{
			opaqueInstances.render();
		}
		
		void Translate(const ObjectID& ID, const Vec3& offset)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				auto& meshID = ID.meshID[i];
				auto& transform = mesh[i].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform;
				transform.position += offset;
				transform.UpdateMatrices();
			}
		}
		void SetPosition(const ObjectID& ID, const Vec3& pos)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				auto& meshID = ID.meshID[i];
				auto& transform = mesh[i].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform;
				transform.position = pos;
				transform.UpdateMatrices();
			}
		}
		void Rotate(const ObjectID& ID, const Quaternion& q)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				auto& meshID = ID.meshID[i];
				auto& transform = mesh[i].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform;
				transform.rotation *= q;
				transform.rotation.normalize();
				transform.UpdateMatrices();
			}
		}
		void Rotate(const ObjectID& ID, const Angles& angles, Vec3 _right, Vec3 _top, Vec3 _forward)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				auto& meshID = ID.meshID[i];
				auto& transform = mesh[i].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform;
				transform.rotation *= Quaternion(angles.roll, _forward);
				transform.rotation *= Quaternion(angles.pitch, _right);
				transform.rotation *= Quaternion(angles.yaw, _top);
				transform.rotation.normalize();
				transform.UpdateMatrices();
			}
		}
		void Scale(const ObjectID& ID, const Vec3& scale)
		{
			auto& mesh = opaqueInstances.m_modelInstances[ID.model_index].meshInstances;
			for (uint32_t i = 0; i < mesh.size(); i++)
			{
				auto& meshID = ID.meshID[i];
				auto& transform = mesh[i].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform;
				transform.scale *= scale;
				transform.UpdateMatrices();
			}
		}
		const Vec3& Get_Top(const ObjectID& ID, uint32_t mesh_index=0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.top();
		}
		const Vec3& Get_Forward(const ObjectID& ID, uint32_t mesh_index = 0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.forward();
		}
		const Vec3& Get_Right(const ObjectID& ID, uint32_t mesh_index = 0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.right();
		}
		const Vec3& Get_Position(const ObjectID& ID, uint32_t mesh_index = 0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.position;
		}
		const Matr<4> Get_TransformMatrix(const ObjectID& ID, uint32_t mesh_index = 0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.transform;
		}
		const Matr<4> Get_TransformInvMatrix(const ObjectID& ID, uint32_t mesh_index = 0)
		{
			auto& meshID = ID.meshID[mesh_index];
			return opaqueInstances.m_modelInstances[ID.model_index].meshInstances[mesh_index].materialInstances[meshID.material_index].instances[meshID.instance_index].m_transform.transformInv;
		}
		void updateInstances();
	};
}