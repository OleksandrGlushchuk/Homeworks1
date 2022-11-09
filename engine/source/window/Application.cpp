#include "Application.h"
#include <iterator>
#include "../render/singletones/SamplerManager.h"
#include "../render/singletones/LightSystem.h"
#include "../render/singletones/ModelManager.h"
#include "../render/singletones/MeshSystem.h"
#include "../render/singletones/ShadowManager.h"
#include "../render/singletones/ParticleSystem.h"
#include "../render/singletones/VegetationSystem.h"
#include "../render/singletones/DecalSystem.h"

const float p_near = 0.01f, p_far = 100.f, fovy = M_PI / 3.f;

namespace engine::windows
{

	void Application::Draw()
	{
		ProcessInput();
		CheckDissolutionObjects();
		CheckIncinerationObjects();
		UpdateCurrentTime();
		wnd.BeginFrame();
		renderer.Render(wnd.m_renderTargetView, camera, m_postProcess, m_currentTime, delta_time);
		wnd.EndFrame();
	}

	void Application::OnChangeWindowSize()
	{
		wnd.OnResize();

		need_to_rotate_camera = false;
		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		camera.updateAspect(aspect);
		renderer.need_to_resize_RTV = true;
		Draw();
	}

	void Application::Init()
	{
		m_currentTime = std::chrono::steady_clock::now();
		renderer.Init(1u);
		m_postProcess.Init(1.5f);
		ShadowManager::instance().SetDirectionalLightDSResolution(2048.f);
		ShadowManager::instance().SetDirectionalLightShadowDistance(40.f);
		ShadowManager::instance().SetPointLightDSResolution(1024.f);

		/*ParticleSystem::instance().AddSmokeEmitter(SmokeEmitter(Vec3(-0.5f, 0, 0.f), Vec3(0.2f, 0.1f, 1.f), 
			1.f, 2.5f, 1.f, 0.15f, 0.3f, 0.7f, 0.2f, 8, 8));
		ParticleSystem::instance().AddSmokeEmitter(SmokeEmitter(Vec3(0.995f, 0.8f, 0.6f), Vec3(0.01f, 0.01f, 0.01f),
			0.5f, 3.f, 0.3f, 0.05f, 0.15f, 0.1f, 0.01f, 8, 8));*/


		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		camera = Camera(fovy, aspect, p_near, p_far);
		LightSystem::instance().setDirectionalLightFrustum(camera);
		camera.setWorldOffset(Vec3(0, 0, -2));

		//VegetationSystem::instance().AddGrassArea(4, 4, 0.6f, { -7,-2.55f,-2 });
		//VegetationSystem::instance().AddGrassArea(2, 6, 0.7f, { -10, -2,-2.5f });

		DecalSystem::instance().SetDecalSize(0.4f);

		//LIGHTS
		{
			engine::LightSystem::instance().addPointLight(Vec3(1, 1, 1), 1.7f, Vec3(0, 1.f, -2.3f), 0.25f,
				engine::ModelManager::instance().GetUnitSphereModel());
			engine::LightSystem::instance().addPointLight(Vec3(1, 0.1f, 1), 1.5f, Vec3(1.3f, -0.5f, -0.5f), 0.1f,
				engine::ModelManager::instance().GetUnitSphereModel());

			engine::LightSystem::instance().addDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(-1, 1, -1));
			//engine::LightSystem::instance().addDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(-1, 1, 1));

			engine::ShadowManager::instance().UpdatePointLightShadowResources(engine::LightSystem::instance().getPointLights().size());
			engine::ShadowManager::instance().UpdateDirectionalLightShadowResources(engine::LightSystem::instance().getDirectionalLights().size());
		}

		//SKY
		{
			renderer.m_sky.Init();
			renderer.m_sky.m_texture.Load(L"engine/assets/Sky/night_street.dds");
			renderer.m_sky.SetEnvironment(L"engine/assets/Sky/night_street_irradiance.dds", 
				L"engine/assets/Sky/night_street_reflectance.dds", 
				L"engine/assets/Sky/night_street_reflection.dds");
		}

		//KNIGHTS
		{
			const auto& KnightModel = engine::ModelManager::instance().LoadModel("engine/assets/Knight/Knight.fbx");

			m_knightMaterial[0].m_colorMap.Load(L"engine/assets/Knight/Fur_BaseColor.dds");
			m_knightMaterial[0].m_normalMap.Load(L"engine/assets/Knight/Fur_Normal.dds");
			m_knightMaterial[0].m_roughnessMap.Load(L"engine/assets/Knight/Fur_Roughness.dds");
			m_knightMaterial[0].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.02f);

			m_knightMaterial[1].m_colorMap.Load(L"engine/assets/Knight/Legs_BaseColor.dds");
			m_knightMaterial[1].m_normalMap.Load(L"engine/assets/Knight/Legs_Normal.dds");
			m_knightMaterial[1].m_roughnessMap.Load(L"engine/assets/Knight/Legs_Roughness.dds");
			m_knightMaterial[1].m_metalnessMap.Load(L"engine/assets/Knight/Legs_Metallic.dds");
			m_knightMaterial[1].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_knightMaterial[2].m_colorMap.Load(L"engine/assets/Knight/Torso_BaseColor.dds");
			m_knightMaterial[2].m_normalMap.Load(L"engine/assets/Knight/Torso_Normal.dds");
			m_knightMaterial[2].m_roughnessMap.Load(L"engine/assets/Knight/Torso_Roughness.dds");
			m_knightMaterial[2].m_metalnessMap.Load(L"engine/assets/Knight/Torso_Metallic.dds");
			m_knightMaterial[2].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_knightMaterial[3].m_colorMap.Load(L"engine/assets/Knight/Head_BaseColor.dds");
			m_knightMaterial[3].m_normalMap.Load(L"engine/assets/Knight/Head_Normal.dds");
			m_knightMaterial[3].m_roughnessMap.Load(L"engine/assets/Knight/Head_Roughness.dds");
			m_knightMaterial[3].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.01f);

			m_knightMaterial[4].m_colorMap.Load(L"engine/assets/Knight/Eye_BaseColor.dds");
			m_knightMaterial[4].m_materialConstantBuffer = MaterialConstantBuffer(false, false, false, false, 0.02f, 0.2f);

			m_knightMaterial[5].m_colorMap.Load(L"engine/assets/Knight/Helmet_BaseColor.dds");
			m_knightMaterial[5].m_normalMap.Load(L"engine/assets/Knight/Helmet_Normal.dds");
			m_knightMaterial[5].m_roughnessMap.Load(L"engine/assets/Knight/Helmet_Roughness.dds");
			m_knightMaterial[5].m_metalnessMap.Load(L"engine/assets/Knight/Helmet_Metallic.dds");
			m_knightMaterial[5].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_knightMaterial[6].m_colorMap.Load(L"engine/assets/Knight/Skirt_BaseColor.dds");
			m_knightMaterial[6].m_normalMap.Load(L"engine/assets/Knight/Skirt_Normal.dds");
			m_knightMaterial[6].m_roughnessMap.Load(L"engine/assets/Knight/Skirt_Roughness.dds");
			m_knightMaterial[6].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.02f);

			m_knightMaterial[7].m_colorMap.Load(L"engine/assets/Knight/Cape_BaseColor.dds");
			m_knightMaterial[7].m_normalMap.Load(L"engine/assets/Knight/Cape_Normal.dds");
			m_knightMaterial[7].m_roughnessMap.Load(L"engine/assets/Knight/Cape_Roughness.dds");
			m_knightMaterial[7].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.02f);

			m_knightMaterial[8].m_colorMap.Load(L"engine/assets/Knight/Glove_BaseColor.dds");
			m_knightMaterial[8].m_normalMap.Load(L"engine/assets/Knight/Glove_Normal.dds");
			m_knightMaterial[8].m_roughnessMap.Load(L"engine/assets/Knight/Glove_Roughness.dds");
			m_knightMaterial[8].m_metalnessMap.Load(L"engine/assets/Knight/Glove_Metallic.dds");
			m_knightMaterial[8].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_dissolubleKnightMaterial = m_knightMaterial;
			m_dissolubleKnightMaterial1 = m_knightMaterial;
			m_dissolubleKnightMaterial1[2].m_colorMap.Load(L"engine/assets/Brick/Brick_Wall_012_COLOR.dds");
			m_dissolubleKnightMaterial2 = m_knightMaterial;
			m_dissolubleKnightMaterial2[2].m_colorMap.Load(L"engine/assets/Samurai/Eye_BaseColor.dds");

			Transform transform = Transform::Identity();
			transform.Translate(Vec3(-1.5f, -0.9f, 0));
			transform.Rotate(Quaternion(M_PI_4, transform.forward()));
			transform.Rotate(Quaternion(M_PI_2, transform.top()));

			engine::MeshSystem::instance().addInstance(KnightModel, m_knightMaterial, OpaqueInstances::Instance(transform));

			transform = Transform::Identity();
			transform.Scale(Vec3(0.5f, 0.5f, 0.5f));
			transform.Translate(Vec3(1.f, 0, 0.6f));
			engine::MeshSystem::instance().addInstance(KnightModel, m_knightMaterial, OpaqueInstances::Instance(transform));
		}

		//SAMURAI
		{
			const auto& SamuraiModel = engine::ModelManager::instance().LoadModel("engine/assets/Samurai/Samurai.fbx");
			std::vector<OpaqueInstances::Material> m(8);
			m_samuraiMaterial[0].m_colorMap.Load(L"engine/assets/Samurai/Sword_BaseColor.dds");
			m_samuraiMaterial[0].m_normalMap.Load(L"engine/assets/Samurai/Sword_Normal.dds");
			m_samuraiMaterial[0].m_roughnessMap.Load(L"engine/assets/Samurai/Sword_Roughness.dds");
			m_samuraiMaterial[0].m_metalnessMap.Load(L"engine/assets/Samurai/Sword_Metallic.dds");
			m_samuraiMaterial[0].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_samuraiMaterial[1].m_colorMap.Load(L"engine/assets/Samurai/Head_BaseColor.dds");
			m_samuraiMaterial[1].m_normalMap.Load(L"engine/assets/Samurai/Head_Normal.dds");
			m_samuraiMaterial[1].m_roughnessMap.Load(L"engine/assets/Samurai/Head_Roughness.dds");
			m_samuraiMaterial[1].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.01f);

			m_samuraiMaterial[2].m_colorMap.Load(L"engine/assets/Samurai/Eye_BaseColor.dds");
			m_samuraiMaterial[2].m_materialConstantBuffer = MaterialConstantBuffer(false, false, false, false, 0.02f, 0.2f);

			m_samuraiMaterial[3].m_colorMap.Load(L"engine/assets/Samurai/Helmet_BaseColor.dds");
			m_samuraiMaterial[3].m_normalMap.Load(L"engine/assets/Samurai/Helmet_Normal.dds");
			m_samuraiMaterial[3].m_roughnessMap.Load(L"engine/assets/Samurai/Helmet_Roughness.dds");
			m_samuraiMaterial[3].m_metalnessMap.Load(L"engine/assets/Samurai/Helmet_Metallic.dds");
			m_samuraiMaterial[3].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_samuraiMaterial[4].m_colorMap.Load(L"engine/assets/Samurai/Torso_BaseColor.dds");
			m_samuraiMaterial[4].m_normalMap.Load(L"engine/assets/Samurai/Torso_Normal.dds");
			m_samuraiMaterial[4].m_roughnessMap.Load(L"engine/assets/Samurai/Torso_Roughness.dds");
			m_samuraiMaterial[4].m_metalnessMap.Load(L"engine/assets/Samurai/Torso_Metallic.dds");
			m_samuraiMaterial[4].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_samuraiMaterial[5].m_colorMap.Load(L"engine/assets/Samurai/Legs_BaseColor.dds");
			m_samuraiMaterial[5].m_normalMap.Load(L"engine/assets/Samurai/Legs_Normal.dds");
			m_samuraiMaterial[5].m_roughnessMap.Load(L"engine/assets/Samurai/Legs_Roughness.dds");
			m_samuraiMaterial[5].m_metalnessMap.Load(L"engine/assets/Samurai/Legs_Metallic.dds");
			m_samuraiMaterial[5].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_samuraiMaterial[6].m_colorMap.Load(L"engine/assets/Samurai/Hand_BaseColor.dds");
			m_samuraiMaterial[6].m_normalMap.Load(L"engine/assets/Samurai/Hand_Normal.dds");
			m_samuraiMaterial[6].m_roughnessMap.Load(L"engine/assets/Samurai/Hand_Roughness.dds");
			m_samuraiMaterial[6].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.02f);

			m_samuraiMaterial[7].m_colorMap.Load(L"engine/assets/Samurai/Torso_BaseColor.dds");
			m_samuraiMaterial[7].m_normalMap.Load(L"engine/assets/Samurai/Torso_Normal.dds");
			m_samuraiMaterial[7].m_roughnessMap.Load(L"engine/assets/Samurai/Torso_Roughness.dds");
			m_samuraiMaterial[7].m_metalnessMap.Load(L"engine/assets/Samurai/Torso_Metallic.dds");
			m_samuraiMaterial[7].m_materialConstantBuffer = MaterialConstantBuffer(true, true, true, false);

			m_dissolubleSamuraiMaterial = m_samuraiMaterial;
			m_dissolubleSamuraiMaterial1 = m_samuraiMaterial;
			m_dissolubleSamuraiMaterial1[7].m_colorMap.Load(L"engine/assets/Stone/Stone_COLOR.dds");

			Transform transform = Transform::Identity();
			transform.Translate(Vec3(3.5f, -1.f, -1.f));
			transform.Scale(Vec3(2, 2, 2));
			transform.Rotate(Quaternion(-M_PI_2, transform.top()));

			engine::MeshSystem::instance().addInstance(SamuraiModel, m_samuraiMaterial, OpaqueInstances::Instance(transform));

			transform = Transform::Identity();
			transform.SetPosition({ 0,4,0 });
			transform.SetScale({ 1.5f,1.5f,1.5f });
			engine::MeshSystem::instance().addInstance(SamuraiModel, engine::EmissiveInstances::Instance({ 0.2f,0.2f,1.f }, transform));
		}

		//CUBES
		{
			std::vector<OpaqueInstances::Material> brick(1);
			brick[0].m_colorMap.Load(L"engine/assets/Brick/Brick_Wall_012_COLOR.dds");
			brick[0].m_normalMap.Load(L"engine/assets/Brick/Brick_Wall_012_NORM.dds");
			brick[0].m_roughnessMap.Load(L"engine/assets/Brick/Brick_Wall_012_ROUGH.dds");
			brick[0].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, false, 0.02f);

			std::vector<OpaqueInstances::Material> stone(1);
			stone[0].m_colorMap.Load(L"engine/assets/Stone/Stone_COLOR.dds");
			stone[0].m_normalMap.Load(L"engine/assets/Stone/Stone_NORM.dds");
			stone[0].m_roughnessMap.Load(L"engine/assets/Stone/Stone_ROUGH.dds");
			stone[0].m_materialConstantBuffer = MaterialConstantBuffer(false, true, true, true, 0.02f);

			Transform transform = Transform::Identity();
			transform.SetScale({ 0.5f, 3, 3 });
			transform.SetPosition({ -2, 0, 0 });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
			
			transform.SetScale({ 0.5, 3, 3 });
			transform.SetPosition({ 2, 0, 0 });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
			
			transform.SetScale({ 4, 3, 0.5f });
			transform.SetPosition({ 0, 0, 1.6f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
			
			transform.SetScale({ 0.3f, 4, 3.8f });
			transform.SetPosition({ -1.4f, 2.2f, 0.21f });
			transform.Rotate(Quaternion(M_PI_4, { 0,0,1 }));
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));

			transform = Transform::Identity();
			transform.SetScale({ 0.3f, 4, 3.8f });
			transform.SetPosition({ 1.4f, 2.2f, 0.2f });
			transform.Rotate(Quaternion(-M_PI_4, { 0,0,1 }));
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
			
			transform = Transform::Identity();
			transform.SetPosition({ 1.f, -0.5f, 1.f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
			
			transform.SetScale({ 4.f, 3.0f, 3.f });
			transform.SetPosition({ 0.f, -3.0f, 0.1f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), stone, OpaqueInstances::Instance(transform));
			

			transform.SetScale({ 0.4f, 2, 0.4f });
			transform.SetPosition({ 1.f, 3.4f, 0.f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));

			transform.SetPosition({ 0,-3,0 });
			transform.SetScale({ 50.f,1.f,50.f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));

			transform.SetPosition({ 0,0,0 });
			transform.SetScale({ 1.f,1.f,1.f });
			transform.Rotate(Quaternion(M_PI / 6.f, transform.top()));
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitQuadModel(), brick, OpaqueInstances::Instance(transform));
		}
	}

	void Application::CheckDissolutionObjects()
	{
		if (time_stopped) return;

		auto& dissolutionInstances = MeshSystem::instance().dissolubleInstances;
		if (dissolutionInstances.m_modelIDs.empty())
			return;

		uint32_t out_materialIndex, out_instanceIndex;
		int transformID = -1;
		float now = std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count();
		for (uint32_t id = 0; id < dissolutionInstances.m_modelIDs.size(); ++id)
		{
			auto modelID = dissolutionInstances.m_modelIDs[id];
			auto& modelInstance = dissolutionInstances.m_modelInstances[modelID.model_index];
			uint32_t meshSize = modelInstance.meshInstances.size();
			std::vector<OpaqueInstances::Material> meshMaterials(meshSize);
			for (uint32_t i = 0; i < meshSize; ++i)
			{
				MeshSystem::instance().GetMaterialAndInstanceIndex<DissolubleInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = modelInstance.meshInstances[i];
				auto& material = meshInstance.materialInstances[out_materialIndex].material;
				auto& instance = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex];
				if (now - instance.creationTime > instance.lifeTime)
				{
					meshMaterials[i] = material;
					transformID = instance.transform_id;
				}
			}
			if (transformID != -1)
			{
				MeshSystem::instance().addInstance(modelInstance.model, meshMaterials, OpaqueInstances::Instance(transformID));
				MeshSystem::instance().deleteInstances<DissolubleInstances>(modelID, false);
				transformID = -1;
			}
		}
	}

	void Application::CheckIncinerationObjects()
	{
		if (time_stopped) return;

		auto& incinerationInstances = MeshSystem::instance().incinerationInstances;
		if (incinerationInstances.m_modelIDs.empty())
			return;

		uint32_t out_materialIndex, out_instanceIndex;
		int transformID = -1;
		float now = std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count();
		for (uint32_t id = 0; id < incinerationInstances.m_modelIDs.size(); ++id)
		{
			auto modelID = incinerationInstances.m_modelIDs[id];
			auto& modelInstance = incinerationInstances.m_modelInstances[modelID.model_index];
			uint32_t meshSize = modelInstance.meshInstances.size();
			for (uint32_t i = 0; i < meshSize; ++i)
			{
				MeshSystem::instance().GetMaterialAndInstanceIndex<IncinerationInstances>(modelID, i, out_materialIndex, out_instanceIndex);
				auto& meshInstance = modelInstance.meshInstances[i];
				auto& material = meshInstance.materialInstances[out_materialIndex].material;
				auto& instance = meshInstance.materialInstances[out_materialIndex].instances[out_instanceIndex];
				if (now - instance.creationTime > instance.lifeTime + delta_time)
				{
					transformID = instance.transform_id;
				}
			}
			if (transformID != -1)
			{
				MeshSystem::instance().deleteInstances<IncinerationInstances>(modelID, true);
				transformID = -1;
			}
		}
	}

	void Application::UpdateCurrentTime()
	{
		if (time_stopped)
			m_currentTime = stop_time_point;
		else
			m_currentTime = std::chrono::steady_clock::now() - stop_duration;
	}

	void Application::ProcessInput()
	{
		Vec3 offset = Vec3(0, 0, 0);
		//MOVEMENT
		{
			if (input_state[VK_SHIFT])
			{
				need_to_speed_up = true;
			}
			else
			{
				need_to_speed_up = false;
			}
			if (input_state['A'])
			{
				offset.e[0] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				need_to_move_camera = true;
			}
			if (input_state['D'])
			{
				offset.e[0] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				need_to_move_camera = true;
			}
			if (input_state['W'])
			{
				offset.e[2] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				need_to_move_camera = true;
			}
			if (input_state['S'])
			{
				offset.e[2] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				need_to_move_camera = true;
			}
			if (input_state['Q'])
			{
				need_to_move_camera = true;
				offset.e[1] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
			}
			if (input_state['E'])
			{
				need_to_move_camera = true;
				offset.e[1] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
			}
		}

		//SAMPLE_FILTRATION
		{
			if (input_state['1'])
			{
				engine::SamplerManager::instance().SetGlobalSamplerState("ss_a");
			}
			if (input_state['2'])
			{
				engine::SamplerManager::instance().SetGlobalSamplerState("ss_mmmp");
			}
			if (input_state['3'])
			{
				engine::SamplerManager::instance().SetGlobalSamplerState("ss_mpmlmp");
			}
			if (input_state['4'])
			{
				engine::SamplerManager::instance().SetGlobalSamplerState("ss_mmlmp");
			}
			if (input_state['5'])
			{
				engine::SamplerManager::instance().SetGlobalSamplerState("ss_mmml");
			}
		}

		//LIGHTING
		{
			if (input_state[VK_OEM_PLUS])
			{
				m_postProcess.EV100 += 1.f * delta_time;
			}
			if (input_state[VK_OEM_MINUS])
			{
				m_postProcess.EV100 -= 1.f * delta_time;
			}
		}

		//OBJECT_ROTATION
		{
			if (input_state['C'])
			{
				if (need_to_rotate_object_relative_to_camera_axes)
				{
					need_to_rotate_object_relative_to_camera_axes = false;
					input_state['C'] = false;
				}
				else
				{
					need_to_rotate_object_relative_to_camera_axes = true;
					input_state['C'] = false;
				}
			}

			if (need_to_move_object)
			{
				Angles rotate_angles;
				if (input_state[VK_LEFT])
				{
					rotate_angles.yaw -= object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_RIGHT])
				{
					rotate_angles.yaw += object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_UP])
				{
					rotate_angles.pitch -= object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_DOWN])
				{
					rotate_angles.pitch += object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_OEM_COMMA])
				{
					rotate_angles.roll -= object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_OEM_PERIOD])
				{
					rotate_angles.roll += object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (need_to_rotate_object)
				{
					rotate_angles.pitch *= delta_time; rotate_angles.roll *= delta_time; rotate_angles.yaw *= delta_time;
					if (need_to_rotate_object_relative_to_camera_axes)
					{
						engine::TransformSystem::instance().m_transforms[clicked_object_transform_id].Rotate(rotate_angles, camera.right(), camera.top(), camera.forward());
					}
					else
					{
						auto& transform = engine::TransformSystem::instance().m_transforms[clicked_object_transform_id];
						transform.Rotate(rotate_angles, transform.right(), transform.top(), transform.forward());
					}
					need_to_rotate_object = false;
				}
			}
		}
		
		//CREATING DISSOLUBLE INSTANCES
		{
			if (input_state['N'])
			{
				const auto& SamuraiModel = engine::ModelManager::instance().LoadModel("engine/assets/Samurai/Samurai.fbx");
				Transform transform = Transform::Identity();
				Vec3 cameraPos = camera.position();
				Vec3 cameraForward = camera.forward();
				transform.SetPosition(cameraPos + 2.f * cameraForward - Vec3(0, 1.f, 0));
				MeshSystem::instance().addInstance(SamuraiModel, m_dissolubleSamuraiMaterial, DissolubleInstances::Instance(transform,
					std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(),
					5.f));
				input_state['N'] = false;
			}
			if (input_state['M'])
			{

				const auto& SamuraiModel = engine::ModelManager::instance().LoadModel("engine/assets/Samurai/Samurai.fbx");
				Transform transform = Transform::Identity();
				Vec3 cameraPos = camera.position();
				Vec3 cameraForward = camera.forward();
				transform.SetPosition(cameraPos + 2.f * cameraForward - Vec3(0, 1.f, 0));
				MeshSystem::instance().addInstance(SamuraiModel, m_dissolubleSamuraiMaterial1, DissolubleInstances::Instance(transform,
					std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(),
					5.f));
				input_state['M'] = false;
			}
			if (input_state['K'])
			{
				const auto& KnightModel = engine::ModelManager::instance().LoadModel("engine/assets/Knight/Knight.fbx");
				Transform transform = Transform::Identity();
				Vec3 cameraPos = camera.position();
				Vec3 cameraForward = camera.forward();
				transform.SetPosition(cameraPos + 2.f * cameraForward - Vec3(0, 1.f, 0));
				MeshSystem::instance().addInstance(KnightModel, m_dissolubleKnightMaterial, DissolubleInstances::Instance(transform,
					std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(),
					5.f));
				input_state['K'] = false;
			}
			if (input_state['J'])
			{
				const auto& KnightModel = engine::ModelManager::instance().LoadModel("engine/assets/Knight/Knight.fbx");
				Transform transform = Transform::Identity();
				Vec3 cameraPos = camera.position();
				Vec3 cameraForward = camera.forward();
				transform.SetPosition(cameraPos + 2.f * cameraForward - Vec3(0, 1.f, 0));
				MeshSystem::instance().addInstance(KnightModel, m_dissolubleKnightMaterial1, DissolubleInstances::Instance(transform,
					std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(),
					5.f));
				input_state['J'] = false;
			}
			if (input_state['H'])
			{
				const auto& KnightModel = engine::ModelManager::instance().LoadModel("engine/assets/Knight/Knight.fbx");
				Transform transform = Transform::Identity();
				Vec3 cameraPos = camera.position();
				Vec3 cameraForward = camera.forward();
				transform.SetPosition(cameraPos + 2.f * cameraForward - Vec3(0, 1.f, 0));
				MeshSystem::instance().addInstance(KnightModel, m_dissolubleKnightMaterial2, DissolubleInstances::Instance(transform,
					std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(),
					5.f));
				input_state['H'] = false;
			}
		}

		//INCINERATION
		{
			if (input_state['O'])
			{
				ray r;
				MeshIntersection outIntersection;
				outIntersection.reset(1);

				float xx = (mouse_x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
				float yy = (mouse_y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;
				r.origin = Vec3(xx, yy, 1);

				float w;
				r.origin.mult(camera.m_viewProjInv, 1, &w);
				r.origin /= w;

				r.direction = r.origin - camera.position();
				r.direction.normalize();

				uint32_t outTransformId;
				uint16_t outModelID;
				std::shared_ptr<Model> model;
				std::vector<OpaqueInstances::Material> material;
				ModelID outObjectID;
				if (MeshSystem::instance().findIntersectionOpaqueEx(r, outIntersection, outTransformId, outModelID, outObjectID))
				{
					model = MeshSystem::instance().opaqueInstances.m_modelInstances[outObjectID.model_index].model;
					auto& meshInstances = MeshSystem::instance().opaqueInstances.m_modelInstances[outObjectID.model_index].meshInstances;

					uint32_t outMaterialIndex, outInstanceIndex;
					for (uint32_t i = 0; i < meshInstances.size(); ++i)
					{
						MeshSystem::instance().GetMaterialAndInstanceIndex<OpaqueInstances>(outObjectID, i, outMaterialIndex, outInstanceIndex);
						material.emplace_back(meshInstances[i].materialInstances[outMaterialIndex].material);
					}

					MeshSystem::instance().deleteInstances<OpaqueInstances>(outObjectID, false);

					const Vec3& scale = TransformSystem::instance().m_transforms[outTransformId].getScale();

					float sphere_max_radius = ((model->box.max - model->box.min) * scale).length();
					float incirenationTime = 7.f;
					float sphere_velocity = sphere_max_radius / incirenationTime;


					MeshSystem::instance().addInstance(model, material, IncinerationInstances::Instance(outTransformId,
						std::chrono::duration_cast<std::chrono::duration<float>>(m_currentTime.time_since_epoch()).count(), incirenationTime,
						outIntersection.pos, sphere_velocity));
				}

				input_state['O'] = false;
			}
		}

		//SPAWNING DECALS
		{
			if (input_state['F'])
			{
				ray r;
				MeshIntersection outIntersection;
				outIntersection.reset(1);

				float xx = (mouse_x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
				float yy = (mouse_y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;
				r.origin = Vec3(xx, yy, 1);

				float w;
				r.origin.mult(camera.m_viewProjInv, 1, &w);
				r.origin /= w;

				r.direction = r.origin - camera.position();
				r.direction.normalize();

				uint32_t outTransformId;
				uint16_t outModelID;

				if (MeshSystem::instance().findIntersectionOpaque(r, outIntersection, outTransformId, outModelID))
					DecalSystem::instance().AddDecalInstance(camera.right(), camera.top(), camera.forward(), outIntersection.pos, outIntersection.normal, outTransformId, outModelID);

				input_state['F'] = false;
			}
			
		}

		if (need_to_rotate_camera)
			RotateCamera();
		if (need_to_move_camera)
		{
			MoveCamera(offset * delta_time);
			need_to_move_camera = false;
		}
		if (need_to_move_object)
			OnRMouseMove();
	}

	void Application::OnKeyDown(WPARAM key)
	{
		input_state[key] = true;
	}

	void Application::OnKeyUp(WPARAM key)
	{
		input_state[key] = false;
	}

	void Application::OnMouseMove(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
	}

	void Application::MoveCamera(const Vec3& offset)
	{
		camera.addRelativeOffset(offset);
		camera.updateMatrices();
	}

	void Application::OnLMouseDown(WORD x, WORD y)
	{
		need_to_rotate_camera = true;
		mouse_x = x;
		mouse_y = y;
		start_rotation.e[0] = end_rotation.e[0] = x;
		start_rotation.e[1] = end_rotation.e[1] = y;
		dir_rotation = Vec3(0, 0, 1);
	}

	void Application::OnLMouseMove()
	{
		end_rotation.e[0] = mouse_x;
		end_rotation.e[1] = mouse_y;
		dir_rotation = delta_time * (start_rotation - end_rotation) * M_PI / wnd.screen.right;
	}

	void Application::OnLMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_rotate_camera = false;
	}

	void Application::OnRMouseDown(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		nearest_clicked_object.reset(1);
		need_to_move_object = false;

		float xx = (x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
		float yy = (y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

		ray_clicked_to_object.origin = Vec3(xx, yy, 1);

		float w;
		ray_clicked_to_object.origin.mult(camera.m_viewProjInv, 1, &w);
		ray_clicked_to_object.origin /= w;

		ray_clicked_to_object.direction = ray_clicked_to_object.origin - camera.position();
		ray_clicked_to_object.direction.normalize();
		

		if (engine::MeshSystem::instance().findIntersection(ray_clicked_to_object, nearest_clicked_object, clicked_object_transform_id))
		{
			distance_object_to_camera = (nearest_clicked_object.pos - camera.position()).length();
			need_to_move_object = true;
		}
	}

	void Application::OnRMouseMove()
	{
		if (need_to_move_object)
		{
			float xx = (mouse_x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
			float yy = (mouse_y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

			ray_clicked_to_object.origin = Vec3(xx, yy, 1);

			float w;
			ray_clicked_to_object.origin.mult(camera.m_viewProjInv, 1, &w);
			ray_clicked_to_object.origin /= w;

			ray_clicked_to_object.direction = ray_clicked_to_object.origin - camera.position();
			ray_clicked_to_object.direction.normalize();

			Vec3 new_position = ray_clicked_to_object.direction * distance_object_to_camera + camera.position();
			engine::TransformSystem::instance().m_transforms[clicked_object_transform_id].Translate(new_position - nearest_clicked_object.pos);
			nearest_clicked_object.pos = new_position;
		}
	}

	void Application::OnRMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_move_object = false;
	}

	void Application::OnEnterSizeMove()
	{
		if (!time_stopped)
		{
			time_stopped = true;
			delta_time = 0;
			stop_time_point = m_currentTime;
		}
	}

	void Application::OnExitSizeMove()
	{
		if (time_stopped)
		{
			time_stopped = false;
			unstop_time_point = std::chrono::steady_clock::now();
			stop_duration = unstop_time_point - stop_time_point;
		}
	}

	void Application::OnMouseWheel(short wheel_data)
	{
		short count = wheel_data / WHEEL_DELTA;
		if (count < 0)
			camera_move_offset_val /= abs(count) * 1.1f;
		else
			camera_move_offset_val *= abs(count) * 1.1f;
	}

	void Application::RotateCamera()
	{
		camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
		camera.updateMatrices();
	}
}