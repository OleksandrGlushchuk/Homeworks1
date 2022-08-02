#include "MeshSystem.h"

namespace engine
{
	MeshSystem* MeshSystem::s_instance;
	void MeshSystem::init()
	{
		ALWAYS_ASSERT(s_instance == nullptr);
		s_instance = new MeshSystem;
		s_instance->opaqueInstances.Init();
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

	void MeshSystem::addInstance(std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, ModelID& out_ID, const Matr<4>& transformMatrix)
	{
		OpaqueInstances::ModelInstance* modelPtr = nullptr;
		for (uint32_t i = 0; i < opaqueInstances.m_modelInstances.size(); ++i)
		{
			if (opaqueInstances.m_modelInstances[i].model->name == model->name)
			{
				modelPtr = &opaqueInstances.m_modelInstances[i];
				out_ID.model_index = i;
				break;
			}
		}
		if (modelPtr == nullptr)
		{
			opaqueInstances.m_modelInstances.push_back(OpaqueInstances::ModelInstance(model, material, transformMatrix));
			
			out_ID.model_index = opaqueInstances.m_modelInstances.size()-1;
			auto& newModel = opaqueInstances.m_modelInstances.back();
			newModel.meshIDs.resize(newModel.model->m_meshes.size() * 2, 0);
			out_ID.meshesBlock_index = 0;

			opaqueInstances.updateInstanceBuffers();
			return;
		}
		else
		{
			out_ID.meshesBlock_index = modelPtr->meshIDs.size();
			modelPtr->meshIDs.resize(modelPtr->meshIDs.size() + modelPtr->model->m_meshes.size() * 2);
			
			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{
				uint32_t material_size = modelPtr->meshInstances[i].materialInstances.size();
				bool need_to_add_material_instance = true;
				for (uint32_t j = 0; j < material_size; ++j)
				{
					if (modelPtr->meshInstances[i].materialInstances[j].material == material[i])
					{
						modelPtr->meshInstances[i].materialInstances[j].instances.push_back(OpaqueInstances::InstancePtr(transformMatrix));
						modelPtr->meshIDs[out_ID.meshesBlock_index + (i * 2)] = j; //MATERIAL_INDEX
						modelPtr->meshIDs[out_ID.meshesBlock_index + (i * 2 + 1)] = modelPtr->meshInstances[i].materialInstances[j].instances.size() - 1; //INSTANCE_INDEX
						need_to_add_material_instance = false;
						break;
					}
				}
				if (need_to_add_material_instance)
				{
					modelPtr->meshInstances[i].materialInstances.push_back(
							OpaqueInstances::MaterialInstance(material[i], transformMatrix));

					modelPtr->meshIDs[out_ID.meshesBlock_index + (i * 2)] = modelPtr->meshInstances[i].materialInstances.size() - 1; //MATERIAL_INDEX
					modelPtr->meshIDs[out_ID.meshesBlock_index + (i * 2 + 1)] = 0; //INSTANCE_INDEX
				}
			}
		}
		opaqueInstances.updateInstanceBuffers();
	}
}