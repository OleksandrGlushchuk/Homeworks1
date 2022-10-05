#pragma once
#include "ModelManager.h"
#include "../OpaqueInstances.h"
#include "../EmissiveInstances.h"
#include "../DissolubleInstances.h"
#include "../GrassField.h"

namespace engine
{
	class MeshSystem
	{
		OpaqueInstances opaqueInstances;
		EmissiveInstances emissiveInstances;

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
	public:
		DissolubleInstances dissolubleInstances;
		GrassField grassField;

		static void init();
		static void deinit();
		static MeshSystem& instance();
		bool findIntersection(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID);
		void addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const OpaqueInstances::Instance &_instance);
		void addInstance(const std::shared_ptr<Model>& model, const EmissiveInstances::Instance &_instance);
		void addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const DissolubleInstances::Instance& _instance);
		
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
		}

		void updateInstances()
		{
			emissiveInstances.updateInstanceBuffers();
			opaqueInstances.updateInstanceBuffers();
			dissolubleInstances.updateInstanceBuffers();
		}

		void renderSceneDepthToCubemaps()
		{
			opaqueInstances.renderSceneDepthToCubemaps();
			grassField.renderSceneDepthToCubemaps();
		}

		void renderSceneDepthForDirectionalLights()
		{
			opaqueInstances.renderSceneDepthForDirectionalLights();
			grassField.renderSceneDepthForDirectionalLights();
		}

		void render()
		{
			dissolubleInstances.render();
			emissiveInstances.render();
			opaqueInstances.render();
		}
	};
}