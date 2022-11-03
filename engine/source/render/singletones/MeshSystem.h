#pragma once
#include "ModelManager.h"
#include "../OpaqueInstances.h"
#include "../EmissiveInstances.h"
#include "../DissolubleInstances.h"
#include "../IncinerationInstances.h"

namespace engine
{
	class MeshSystem
	{
		template<typename InstanceType>
		void GetMaterialAndInstancePointerToIndex(const ModelID& ID, uint32_t mesh_index, uint32_t *& out_material_index, uint32_t *& out_instance_index)
		{
			if constexpr (std::is_same<InstanceType, OpaqueInstances>::value)
			{
				auto& model = opaqueInstances.m_modelInstances[ID.model_index];
				out_material_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
			else if constexpr (std::is_same<InstanceType, DissolubleInstances>::value)
			{
				auto& model = dissolubleInstances.m_modelInstances[ID.model_index];
				out_material_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
			else if constexpr (std::is_same<InstanceType, IncinerationInstances>::value)
			{
				auto& model = incinerationInstances.m_modelInstances[ID.model_index];
				out_material_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = &model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
		}

		static MeshSystem* s_instance;
		MeshSystem() {}
		MeshSystem(const MeshSystem& other) = delete;
		MeshSystem& operator=(const MeshSystem& other) = delete;
		MeshSystem(MeshSystem&& other) noexcept = delete;
		MeshSystem& operator=(MeshSystem&& other) noexcept = delete;
		void GetInstanceIndex(const ModelID& ID, uint32_t mesh_index, uint32_t& out_instance_index) const
		{
			auto& model = emissiveInstances.m_modelInstances[ID.model_index];
			out_instance_index = model.meshIDs[ID.meshesBlock_index + mesh_index];
		}
		uint16_t m_modelIDsCounter = 1; //START ID = 1; 0 for unused
	public:
		DissolubleInstances dissolubleInstances;
		OpaqueInstances opaqueInstances;
		EmissiveInstances emissiveInstances;
		IncinerationInstances incinerationInstances;

		static void init();
		static void deinit();
		static MeshSystem& instance();
		bool findIntersection(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID);
		bool findIntersectionOpaque(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID, uint16_t& out_modelID);
		bool findIntersectionOpaqueEx(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID, uint16_t& out_modelID, ModelID& outObjectID);

		void addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const OpaqueInstances::Instance &_instance);
		void addInstance(const std::shared_ptr<Model>& model, const EmissiveInstances::Instance &_instance);
		void addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const DissolubleInstances::Instance& _instance);
		void addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const IncinerationInstances::Instance& _instance);

		template<typename InstanceType>
		void deleteInstances(const ModelID& del_objectID, bool need_to_delete_transform);

		template<typename InstanceType>
		void GetMaterialAndInstanceIndex(const ModelID& ID, uint32_t mesh_index, uint32_t& out_material_index, uint32_t& out_instance_index) const
		{
			if constexpr (std::is_same<InstanceType, OpaqueInstances>::value)
			{
				auto& model = opaqueInstances.m_modelInstances[ID.model_index];
				out_material_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
			else if constexpr (std::is_same<InstanceType, DissolubleInstances>::value)
			{
				auto& model = dissolubleInstances.m_modelInstances[ID.model_index];
				out_material_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
			else if constexpr (std::is_same<InstanceType, IncinerationInstances>::value)
			{
				auto& model = incinerationInstances.m_modelInstances[ID.model_index];
				out_material_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2)];
				out_instance_index = model.meshIDs[ID.meshesBlock_index + (mesh_index * 2 + 1)];
			}
		}

		void updateInstances()
		{
			emissiveInstances.updateInstanceBuffers();
			opaqueInstances.updateInstanceBuffers();
			dissolubleInstances.updateInstanceBuffers();
			incinerationInstances.updateInstanceBuffers();
		}

		void renderSceneDepthToCubemaps()
		{
			opaqueInstances.renderSceneDepthToCubemaps();
			dissolubleInstances.renderSceneDepthToCubemaps();
		}

		void renderSceneDepthForDirectionalLights()
		{
			opaqueInstances.renderSceneDepthForDirectionalLights();
			dissolubleInstances.renderSceneDepthForDirectionalLights();
		}

		void render()
		{
			dissolubleInstances.render();
			opaqueInstances.render();
			incinerationInstances.render();
		}
	};


	template<typename InstanceType>
	inline void MeshSystem::deleteInstances(const ModelID& del_objectID, bool need_to_delete_transform)
	{
		InstanceType* p_instancesType = nullptr;
		if constexpr (std::is_same<InstanceType, OpaqueInstances>::value)
		{
			p_instancesType = &opaqueInstances;
		}
		else if constexpr (std::is_same<InstanceType, DissolubleInstances>::value)
		{
			p_instancesType = &dissolubleInstances;
		}
		else if constexpr (std::is_same<InstanceType, IncinerationInstances>::value)
		{
			p_instancesType = &incinerationInstances;
		}

		auto& instancesType = *p_instancesType;

		bool need_to_reduce_material_instances = false, need_to_reduce_model_instances = false;
		uint32_t del_materialIndex, del_instanceIndex;
		auto& del_modelInstance = instancesType.m_modelInstances[del_objectID.model_index];
		uint32_t del_meshInstancesNum = del_modelInstance.meshInstances.size();
		uint32_t deletedMeshNum = 0, transformID;

		for (uint32_t i = 0; i < del_meshInstancesNum; ++i)
		{
			GetMaterialAndInstanceIndex<InstanceType>(del_objectID, i, del_materialIndex, del_instanceIndex);
			auto& del_meshInstance = del_modelInstance.meshInstances[i - deletedMeshNum];
			auto& del_materialInstance = del_meshInstance.materialInstances[del_materialIndex];

			transformID = del_materialInstance.instances[del_instanceIndex].transform_id;
			del_materialInstance.instances.erase(del_materialInstance.instances.begin() + del_instanceIndex);

			if (del_materialInstance.instances.empty())
			{
				del_meshInstance.materialInstances.erase(del_meshInstance.materialInstances.begin() + del_materialIndex);
				need_to_reduce_material_instances = true;

				if (del_meshInstance.materialInstances.empty())
				{
					del_modelInstance.meshInstances.erase(del_modelInstance.meshInstances.begin() + (i - deletedMeshNum));
					++deletedMeshNum;
				}
			}
		}
		if (del_modelInstance.meshInstances.empty())
		{
			instancesType.m_modelInstances.erase(instancesType.m_modelInstances.begin() + del_objectID.model_index);
			need_to_reduce_model_instances = true;
		}

		uint32_t* p_materialIndex, * p_instanceIndex;
		uint32_t del_objectID_Index;

		for (uint32_t i = 0; i < instancesType.m_modelIDs.size(); ++i)
		{
			auto& ID = instancesType.m_modelIDs[i];

			if (ID.model_index == del_objectID.model_index)
			{
				if (ID.meshesBlock_index != del_objectID.meshesBlock_index)
				{
					for (uint32_t k = 0; k < del_meshInstancesNum; ++k)
					{
						GetMaterialAndInstanceIndex<InstanceType>(del_objectID, k, del_materialIndex, del_instanceIndex);
						GetMaterialAndInstancePointerToIndex<InstanceType>(ID, k, p_materialIndex, p_instanceIndex);

						if ((*p_materialIndex) == del_materialIndex)
						{
							if ((*p_instanceIndex) > del_instanceIndex)
								(*p_instanceIndex) -= 1;
						}
						else if ((*p_materialIndex) > del_materialIndex)
						{
							if (need_to_reduce_material_instances)
								(*p_materialIndex) -= 1;
						}
					}
				}
				else
					del_objectID_Index = i;

				if (ID.meshesBlock_index > del_objectID.meshesBlock_index)
					ID.meshesBlock_index -= 2 * del_meshInstancesNum;
			}
			else if (ID.model_index > del_objectID.model_index)
			{
				if (need_to_reduce_model_instances)
					ID.model_index -= 1;
			}
		}

		if (need_to_delete_transform)
			TransformSystem::instance().m_transforms.erase(transformID);

		instancesType.m_modelIDs.erase(instancesType.m_modelIDs.begin() + del_objectID_Index);

		if (!need_to_reduce_model_instances)
			if (!instancesType.m_modelInstances.empty())
			{
				del_modelInstance.meshIDs.erase(
					del_modelInstance.meshIDs.begin() + del_objectID.meshesBlock_index,
					del_modelInstance.meshIDs.begin() + (del_objectID.meshesBlock_index + 2 * del_meshInstancesNum));
			}
	}
	
}