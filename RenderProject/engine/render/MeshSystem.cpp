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

	void MeshSystem::addInstance(std::shared_ptr<Model>& model, const std::vector<OpaqueInstances::Material>& material, ObjectID& out_ID)
	{
		OpaqueInstances::ModelInstance* modelPtr = nullptr;
		out_ID.meshID.resize(model->m_meshes.size());
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
			opaqueInstances.m_modelInstances.push_back(OpaqueInstances::ModelInstance(model, material));
			opaqueInstances.updateInstanceBuffers();
			out_ID.model_index = opaqueInstances.m_modelInstances.size()-1;
			for (auto& meshID : out_ID.meshID)
			{
				meshID.instance_index = meshID.material_index = 0;
			}
			return;
		}
		else
		{
			for (uint32_t i = 0; i < modelPtr->meshInstances.size(); ++i)
			{

				uint32_t material_size = modelPtr->meshInstances[i].materialInstances.size();
				bool need_to_add_material_instance = false;
				for (uint32_t j = 0; j < material_size; ++j)
				{
					if (modelPtr->meshInstances[i].materialInstances[j].material == material[i])
					{
						modelPtr->meshInstances[i].materialInstances[j].instances.push_back(OpaqueInstances::InstancePtr(model->m_meshes[i].meshToModelMatrix));

						out_ID.meshID[i].material_index = j;
						out_ID.meshID[i].instance_index = modelPtr->meshInstances[i].materialInstances[j].instances.size() - 1;
						need_to_add_material_instance = false;
						break;
					}
					else
					{
						need_to_add_material_instance = true;
					}
				}
				if (need_to_add_material_instance)
				{
					modelPtr->meshInstances[i].materialInstances.push_back(
							OpaqueInstances::MaterialInstance(material[i], model->m_meshes[i].meshToModelMatrix));

						out_ID.meshID[i].material_index = modelPtr->meshInstances[i].materialInstances.size() - 1;
						out_ID.meshID[i].instance_index = 0;
				}
			}
		}
		opaqueInstances.updateInstanceBuffers();
	}
	void MeshSystem::updateInstances()
	{
		opaqueInstances.updateInstanceBuffers();
	}
}