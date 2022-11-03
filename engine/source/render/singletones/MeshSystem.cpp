#include "MeshSystem.h"
#include "LightSystem.h"

namespace engine
{
	MeshSystem* MeshSystem::s_instance;
	void MeshSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new MeshSystem;
		s_instance->opaqueInstances.Init();
		s_instance->emissiveInstances.Init();
		s_instance->dissolubleInstances.Init();
		s_instance->incinerationInstances.Init();
	}

	void MeshSystem::deinit()
	{
		ALWAYS_ASSERT(s_instance != nullptr);
		delete s_instance;
		s_instance = nullptr;
	}

	MeshSystem& MeshSystem::instance()
	{
		ALWAYS_ASSERT(s_instance); return *s_instance;
	}

	bool MeshSystem::findIntersection(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t &out_transformID)
	{
		uint32_t out_materialIndex, out_instanceIndex, transformID;
		bool result = false;
		const Matr<4>* transform = nullptr;
		const Matr<4>* mesh_to_model = nullptr;
		for (ModelID& modelID : opaqueInstances.m_modelIDs)
		{
			auto& model = opaqueInstances.m_modelInstances[modelID.model_index];
			for (uint32_t i = 0; i < model.meshInstances.size(); ++i)
			{
				GetMaterialAndInstanceIndex<OpaqueInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = model.meshInstances[i];

				transformID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].transform_id;
				const Matr<4>& transformInv = engine::TransformSystem::instance().m_transforms[transformID].getTransformInvMatrix();
				Matr<4> InvMatr = transformInv * model.model->m_meshes[i].meshToModelMatrix.invert();

				ray model_ray = _ray;
				model_ray.origin.mult(InvMatr);
				model_ray.direction.mult(InvMatr, 0);

				if (model.model->m_meshes[i].triangleOctree.intersect(model_ray, out_intersection))
				{
					out_transformID = transformID;
					transform = &engine::TransformSystem::instance().m_transforms[transformID].getTransformMatrix();
					mesh_to_model = &model.model->m_meshes[i].meshToModelMatrix;
					result = true;
				}
			}
		}

		for (ModelID& modelID : dissolubleInstances.m_modelIDs)
		{
			auto& model = dissolubleInstances.m_modelInstances[modelID.model_index];
			for (uint32_t i = 0; i < model.meshInstances.size(); ++i)
			{
				GetMaterialAndInstanceIndex<DissolubleInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = model.meshInstances[i];

				transformID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].transform_id;
				const Matr<4>& transformInv = engine::TransformSystem::instance().m_transforms[transformID].getTransformInvMatrix();
				Matr<4> InvMatr = transformInv * model.model->m_meshes[i].meshToModelMatrix.invert();

				ray model_ray = _ray;
				model_ray.origin.mult(InvMatr);
				model_ray.direction.mult(InvMatr, 0);

				if (model.model->m_meshes[i].triangleOctree.intersect(model_ray, out_intersection))
				{
					out_transformID = transformID;
					transform = &engine::TransformSystem::instance().m_transforms[transformID].getTransformMatrix();
					mesh_to_model = &model.model->m_meshes[i].meshToModelMatrix;
					result = true;
				}
			}
		}

		for (ModelID& modelID : emissiveInstances.m_modelIDs)
		{
			auto& model = emissiveInstances.m_modelInstances[modelID.model_index];
			for (uint32_t i = 0; i < model.meshInstances.size(); ++i)
			{
				GetInstanceIndex(modelID, i, out_instanceIndex);
				auto& meshInstance = model.meshInstances[i];

				transformID = meshInstance.instances[out_instanceIndex].transform_id;
				const Matr<4>& transformInv = engine::TransformSystem::instance().m_transforms[transformID].getTransformInvMatrix();
				ray model_ray = _ray;
				Matr<4> InvMatr = transformInv * model.model->m_meshes[i].meshToModelMatrix.invert();
				model_ray.origin.mult(InvMatr);
				model_ray.direction.mult(InvMatr, 0);

				if (model.model->m_meshes[i].triangleOctree.intersect(model_ray, out_intersection))
				{
					out_transformID = transformID;
					transform = &engine::TransformSystem::instance().m_transforms[transformID].getTransformMatrix();
					mesh_to_model = &model.model->m_meshes[i].meshToModelMatrix;
					result = true;
				}
			}
		}
		if (transform && mesh_to_model)
			out_intersection.pos.mult((*mesh_to_model) * (*transform));
		return result;
	}

	bool MeshSystem::findIntersectionOpaque(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID, uint16_t &out_modelID)
	{
		uint32_t out_materialIndex, out_instanceIndex, transformID;
		bool result = false;
		const Matr<4>* transform = nullptr;
		const Matr<4>* mesh_to_model = nullptr;
		for (ModelID& modelID : opaqueInstances.m_modelIDs)
		{
			auto& model = opaqueInstances.m_modelInstances[modelID.model_index];
			for (uint32_t i = 0; i < model.meshInstances.size(); ++i)
			{
				GetMaterialAndInstanceIndex<OpaqueInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = model.meshInstances[i];

				transformID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].transform_id;
				const Matr<4>& transformInv = engine::TransformSystem::instance().m_transforms[transformID].getTransformInvMatrix();
				Matr<4> InvMatr = transformInv * model.model->m_meshes[i].meshToModelMatrix.invert();

				ray model_ray = _ray;
				model_ray.origin.mult(InvMatr);
				model_ray.direction.mult(InvMatr, 0);

				if (model.model->m_meshes[i].triangleOctree.intersect(model_ray, out_intersection))
				{
					out_transformID = transformID;
					out_modelID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].modelID;
					transform = &engine::TransformSystem::instance().m_transforms[transformID].getTransformMatrix();
					mesh_to_model = &model.model->m_meshes[i].meshToModelMatrix;
					result = true;
				}
			}
		}
		if (transform && mesh_to_model)
			out_intersection.pos.mult((*mesh_to_model) * (*transform));
		return result;
	}

	bool MeshSystem::findIntersectionOpaqueEx(const ray& _ray, engine::MeshIntersection& out_intersection, uint32_t& out_transformID, uint16_t& out_modelID, ModelID& outObjectID)
	{
		uint32_t out_materialIndex, out_instanceIndex, transformID;
		bool result = false;
		const Matr<4>* transform = nullptr;
		const Matr<4>* mesh_to_model = nullptr;

		for (ModelID& modelID : opaqueInstances.m_modelIDs)
		{
			auto& model = opaqueInstances.m_modelInstances[modelID.model_index];
			for (uint32_t i = 0; i < model.meshInstances.size(); ++i)
			{
				GetMaterialAndInstanceIndex<OpaqueInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = model.meshInstances[i];

				transformID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].transform_id;
				const Matr<4>& transformInv = engine::TransformSystem::instance().m_transforms[transformID].getTransformInvMatrix();
				Matr<4> InvMatr = transformInv * model.model->m_meshes[i].meshToModelMatrix.invert();

				ray model_ray = _ray;
				model_ray.origin.mult(InvMatr);
				model_ray.direction.mult(InvMatr, 0);

				if (model.model->m_meshes[i].triangleOctree.intersect(model_ray, out_intersection))
				{
					out_transformID = transformID;
					out_modelID = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex].modelID;;

					transform = &engine::TransformSystem::instance().m_transforms[transformID].getTransformMatrix();
					mesh_to_model = &model.model->m_meshes[i].meshToModelMatrix;
					result = true;
					outObjectID = modelID;
				}
			}
		}
		if (transform && mesh_to_model)
		{
			out_intersection.pos.mult((*mesh_to_model) * (*transform));
		}
		return result;
	}

	void MeshSystem::addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const OpaqueInstances::Instance& _instance)
	{
		OpaqueInstances::ModelInstance* modelPtr = nullptr;
		opaqueInstances.m_modelIDs.emplace_back();
		auto& modelID = opaqueInstances.m_modelIDs.back();
		for (uint32_t i = 0; i < opaqueInstances.m_modelInstances.size(); ++i)
		{
			if (opaqueInstances.m_modelInstances[i].model->name == model->name)
			{
				modelPtr = &opaqueInstances.m_modelInstances[i];
				modelID.model_index = i;
				break;
			}
		}
		if (modelPtr == nullptr)
		{
			auto& mod_inst = opaqueInstances.m_modelInstances.emplace_back(OpaqueInstances::ModelInstance(model, material, _instance));
			for (auto& mesh : mod_inst.meshInstances)
			{
				for (auto& inst : mesh.materialInstances[0].instances)
				{
					inst.modelID = m_modelIDsCounter;
				}
			}

			modelID.model_index = opaqueInstances.m_modelInstances.size()-1;
			auto& newModel = opaqueInstances.m_modelInstances.back();
			newModel.meshIDs.resize(newModel.model->m_meshes.size() * 2, 0);
			modelID.meshesBlock_index = 0;
		}
		else
		{
			modelID.meshesBlock_index = modelPtr->meshIDs.size();
			modelPtr->meshIDs.resize(modelPtr->meshIDs.size() + modelPtr->model->m_meshes.size() * 2);
			
			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{
				uint32_t material_size = modelPtr->meshInstances[i].materialInstances.size();
				bool need_to_add_material_instance = true;
				for (uint32_t j = 0; j < material_size; ++j)
				{
					if (modelPtr->meshInstances[i].materialInstances[j].material == material[i])
					{
						auto& inst = modelPtr->meshInstances[i].materialInstances[j].instances.emplace_back(_instance);
						inst.modelID = m_modelIDsCounter;
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = j; //MATERIAL_INDEX
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = modelPtr->meshInstances[i].materialInstances[j].instances.size() - 1; //INSTANCE_INDEX
						need_to_add_material_instance = false;
						break;
					}
				}
				if (need_to_add_material_instance)
				{
					auto& mat_inst = modelPtr->meshInstances[i].materialInstances.emplace_back(
							OpaqueInstances::MaterialInstance(material[i], _instance));
					mat_inst.instances[0].modelID = m_modelIDsCounter;
					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = modelPtr->meshInstances[i].materialInstances.size() - 1; //MATERIAL_INDEX
					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = 0; //INSTANCE_INDEX
				}
			}
		}
		++m_modelIDsCounter;
		opaqueInstances.updateInstanceBuffers();
	}

	void MeshSystem::addInstance(const std::shared_ptr<Model>& model, const EmissiveInstances::Instance& _instance)
	{
		EmissiveInstances::ModelInstance* modelPtr = nullptr;
		emissiveInstances.m_modelIDs.emplace_back();
		auto& modelID = emissiveInstances.m_modelIDs.back();

		for (uint32_t i = 0; i < emissiveInstances.m_modelInstances.size(); ++i)
		{
			if (emissiveInstances.m_modelInstances[i].model->name == model->name)
			{
				modelPtr = &emissiveInstances.m_modelInstances[i];
				modelID.model_index = i;
				break;
			}
		}
		if (modelPtr == nullptr)
		{
			emissiveInstances.m_modelInstances.push_back(EmissiveInstances::ModelInstance(model, _instance));

			modelID.model_index = emissiveInstances.m_modelInstances.size() - 1;
			auto& newModel = emissiveInstances.m_modelInstances.back();
			newModel.meshIDs.resize(newModel.model->m_meshes.size(), 0);
			modelID.meshesBlock_index = 0;
		}
		else
		{
			modelID.meshesBlock_index = modelPtr->meshIDs.size();
			modelPtr->meshIDs.resize(modelPtr->meshIDs.size() + modelPtr->model->m_meshes.size());

			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{
				modelPtr->meshInstances[i].instances.push_back(_instance);
				modelPtr->meshIDs[modelID.meshesBlock_index + (i)] = modelPtr->meshInstances[i].instances.size() - 1; //INSTANCE_INDEX
			}
		}
		emissiveInstances.updateInstanceBuffers();
	}

	void MeshSystem::addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const DissolubleInstances::Instance& _instance)
	{
		DissolubleInstances::ModelInstance* modelPtr = nullptr;
		dissolubleInstances.m_modelIDs.emplace_back();
		auto& modelID = dissolubleInstances.m_modelIDs.back();

		for (uint32_t i = 0; i < dissolubleInstances.m_modelInstances.size(); ++i)
		{
			if (dissolubleInstances.m_modelInstances[i].model->name == model->name)
			{
				modelPtr = &dissolubleInstances.m_modelInstances[i];
				modelID.model_index = i;
				break;
			}
		}
		if (modelPtr == nullptr)
		{
			dissolubleInstances.m_modelInstances.push_back(DissolubleInstances::ModelInstance(model, material, _instance));
			modelID.model_index = dissolubleInstances.m_modelInstances.size() - 1;
			auto& newModel = dissolubleInstances.m_modelInstances.back();
			newModel.meshIDs.resize(newModel.model->m_meshes.size() * 2, 0);
			modelID.meshesBlock_index = 0;
		}
		else
		{
			modelID.meshesBlock_index = modelPtr->meshIDs.size();
			modelPtr->meshIDs.resize(modelPtr->meshIDs.size() + modelPtr->model->m_meshes.size() * 2);

			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{
				uint32_t material_size = modelPtr->meshInstances[i].materialInstances.size();
				bool need_to_add_material_instance = true;
				for (uint32_t j = 0; j < material_size; ++j)
				{
					if (modelPtr->meshInstances[i].materialInstances[j].material == material[i])
					{
						modelPtr->meshInstances[i].materialInstances[j].instances.push_back(_instance);
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = j; //MATERIAL_INDEX
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = modelPtr->meshInstances[i].materialInstances[j].instances.size() - 1; //INSTANCE_INDEX
						need_to_add_material_instance = false;
						break;
					}
				}
				if (need_to_add_material_instance)
				{
					modelPtr->meshInstances[i].materialInstances.push_back(
						DissolubleInstances::MaterialInstance(material[i], _instance));

					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = modelPtr->meshInstances[i].materialInstances.size() - 1; //MATERIAL_INDEX
					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = 0; //INSTANCE_INDEX
				}
			}
		}
		dissolubleInstances.updateInstanceBuffers();
	}

	void MeshSystem::addInstance(const std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, const IncinerationInstances::Instance& _instance)
	{
		IncinerationInstances::ModelInstance* modelPtr = nullptr;
		incinerationInstances.m_modelIDs.emplace_back();
		auto& modelID = incinerationInstances.m_modelIDs.back();

		for (uint32_t i = 0; i < incinerationInstances.m_modelInstances.size(); ++i)
		{
			if (incinerationInstances.m_modelInstances[i].model->name == model->name)
			{
				modelPtr = &incinerationInstances.m_modelInstances[i];
				modelID.model_index = i;
				break;
			}
		}
		if (modelPtr == nullptr)
		{
			incinerationInstances.m_modelInstances.push_back(IncinerationInstances::ModelInstance(model, material, _instance));
			modelID.model_index = incinerationInstances.m_modelInstances.size() - 1;
			auto& newModel = incinerationInstances.m_modelInstances.back();
			newModel.meshIDs.resize(newModel.model->m_meshes.size() * 2, 0);
			modelID.meshesBlock_index = 0;
		}
		else
		{
			modelID.meshesBlock_index = modelPtr->meshIDs.size();
			modelPtr->meshIDs.resize(modelPtr->meshIDs.size() + modelPtr->model->m_meshes.size() * 2);

			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{
				uint32_t material_size = modelPtr->meshInstances[i].materialInstances.size();
				bool need_to_add_material_instance = true;
				for (uint32_t j = 0; j < material_size; ++j)
				{
					if (modelPtr->meshInstances[i].materialInstances[j].material == material[i])
					{
						modelPtr->meshInstances[i].materialInstances[j].instances.push_back(_instance);
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = j; //MATERIAL_INDEX
						modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = modelPtr->meshInstances[i].materialInstances[j].instances.size() - 1; //INSTANCE_INDEX
						need_to_add_material_instance = false;
						break;
					}
				}
				if (need_to_add_material_instance)
				{
					modelPtr->meshInstances[i].materialInstances.push_back(
						IncinerationInstances::MaterialInstance(material[i], _instance));

					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2)] = modelPtr->meshInstances[i].materialInstances.size() - 1; //MATERIAL_INDEX
					modelPtr->meshIDs[modelID.meshesBlock_index + (i * 2 + 1)] = 0; //INSTANCE_INDEX
				}
			}
		}
		incinerationInstances.updateInstanceBuffers();
	}
}