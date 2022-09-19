#include "Application.h"
#include "../render/SamplerManager.h"
#include "../render/LightSystem.h"
#include "../render/ModelManager.h"
#include "../render/MeshSystem.h"
#include "../render/ShadowManager.h"

const float p_near = 0.01f, p_far = 150.f, fovy = M_PI / 3.f;

namespace engine::windows
{

	void Application::Draw()
	{
		ProcessInput();
		wnd.BeginFrame();
		renderer.Render(wnd.m_renderTargetView, camera, m_postProcess);
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
		renderer.Init(4u);
		m_postProcess.Init();
		ShadowManager::instance().SetDirectionalLightDSResolution(4096.f);
		ShadowManager::instance().SetPointLightDSResolution(1024.f);


		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		camera = Camera(fovy, aspect, p_near, p_far);
		camera.setWorldOffset(Vec3(0, 0, -2));
		LightSystem::instance().setDirectionalLightFrustum(camera);

		//LIGHTS
		{
			engine::LightSystem::instance().addPointLight(Vec3(1, 1, 1), 1.7f, Vec3(0, 1.f, -2.3f), 0.25f,
				engine::ModelManager::instance().GetUnitSphereModel());
			engine::LightSystem::instance().addPointLight(Vec3(1, 0.1f, 1), 1.1f, Vec3(1.3f, -0.5f, -0.5f), 0.1f,
				engine::ModelManager::instance().GetUnitSphereModel());

			engine::LightSystem::instance().addDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(-1, 1, -1));
			engine::LightSystem::instance().addDirectionalLight(Vec3(1.f, 1.f, 1.f), Vec3(-1, 1, 1));

			engine::ShadowManager::instance().UpdatePointLightShadowResources(engine::LightSystem::instance().getPointLights().size());
			engine::ShadowManager::instance().UpdateDirectionalLightShadowResources(engine::LightSystem::instance().getDirectionalLights().size());
		}

		//SKY
		{
			renderer.m_sky.Init();
			renderer.m_sky.m_texture.Load(L"source/assets/Sky/night_street.dds");
			renderer.m_sky.SetEnvironment(L"source/assets/Sky/night_street_irradiance.dds", 
				L"source/assets/Sky/night_street_reflectance.dds", 
				L"source/assets/Sky/night_street_reflection.dds");
		}
		//KNIGHTS
		{
			const auto& KnightModel = engine::ModelManager::instance().LoadModel("source/assets/Knight/Knight.fbx");

			std::vector<OpaqueInstances::Material> m(9);
			m[0].m_colorMap.Load(L"source/assets/Knight/Fur_BaseColor.dds");
			m[0].m_normalMap.Load(L"source/assets/Knight/Fur_Normal.dds");
			m[0].m_roughnessMap.Load(L"source/assets/Knight/Fur_Roughness.dds");
			m[0].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.02f);

			m[1].m_colorMap.Load(L"source/assets/Knight/Legs_BaseColor.dds");
			m[1].m_normalMap.Load(L"source/assets/Knight/Legs_Normal.dds");
			m[1].m_roughnessMap.Load(L"source/assets/Knight/Legs_Roughness.dds");
			m[1].m_metalnessMap.Load(L"source/assets/Knight/Legs_Metallic.dds");
			m[1].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[2].m_colorMap.Load(L"source/assets/Knight/Torso_BaseColor.dds");
			m[2].m_normalMap.Load(L"source/assets/Knight/Torso_Normal.dds");
			m[2].m_roughnessMap.Load(L"source/assets/Knight/Torso_Roughness.dds");
			m[2].m_metalnessMap.Load(L"source/assets/Knight/Torso_Metallic.dds");
			m[2].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[3].m_colorMap.Load(L"source/assets/Knight/Head_BaseColor.dds");
			m[3].m_normalMap.Load(L"source/assets/Knight/Head_Normal.dds");
			m[3].m_roughnessMap.Load(L"source/assets/Knight/Head_Roughness.dds");
			m[3].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.01f);

			m[4].m_colorMap.Load(L"source/assets/Knight/Eye_BaseColor.dds");
			m[4].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, false, false, false, 0.02f, 0.2f);

			m[5].m_colorMap.Load(L"source/assets/Knight/Helmet_BaseColor.dds");
			m[5].m_normalMap.Load(L"source/assets/Knight/Helmet_Normal.dds");
			m[5].m_roughnessMap.Load(L"source/assets/Knight/Helmet_Roughness.dds");
			m[5].m_metalnessMap.Load(L"source/assets/Knight/Helmet_Metallic.dds");
			m[5].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[6].m_colorMap.Load(L"source/assets/Knight/Skirt_BaseColor.dds");
			m[6].m_normalMap.Load(L"source/assets/Knight/Skirt_Normal.dds");
			m[6].m_roughnessMap.Load(L"source/assets/Knight/Skirt_Roughness.dds");
			m[6].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.02f);

			m[7].m_colorMap.Load(L"source/assets/Knight/Cape_BaseColor.dds");
			m[7].m_normalMap.Load(L"source/assets/Knight/Cape_Normal.dds");
			m[7].m_roughnessMap.Load(L"source/assets/Knight/Cape_Roughness.dds");
			m[7].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.02f);

			m[8].m_colorMap.Load(L"source/assets/Knight/Glove_BaseColor.dds");
			m[8].m_normalMap.Load(L"source/assets/Knight/Glove_Normal.dds");
			m[8].m_roughnessMap.Load(L"source/assets/Knight/Glove_Roughness.dds");
			m[8].m_metalnessMap.Load(L"source/assets/Knight/Glove_Metallic.dds");
			m[8].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			Transform transform = Transform::Identity();
			transform.Translate(Vec3(-1.5f, -0.9f, 0));
			transform.Rotate(Quaternion(M_PI_4, transform.forward()));
			transform.Rotate(Quaternion(M_PI_2, transform.top()));

			engine::MeshSystem::instance().addInstance(KnightModel, m, OpaqueInstances::Instance(transform));

			transform = Transform::Identity();
			transform.Scale(Vec3(0.5f, 0.5f, 0.5f));
			transform.Translate(Vec3(1.f, 0, 0.6f));
			engine::MeshSystem::instance().addInstance(KnightModel, m, OpaqueInstances::Instance(transform));
		}

		//SAMURAI
		{
			const auto& SamuraiModel = engine::ModelManager::instance().LoadModel("source/assets/Samurai/Samurai.fbx");
			std::vector<OpaqueInstances::Material> m(8);
			m[0].m_colorMap.Load(L"source/assets/Samurai/Sword_BaseColor.dds");
			m[0].m_normalMap.Load(L"source/assets/Samurai/Sword_Normal.dds");
			m[0].m_roughnessMap.Load(L"source/assets/Samurai/Sword_Roughness.dds");
			m[0].m_metalnessMap.Load(L"source/assets/Samurai/Sword_Metallic.dds");
			m[0].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[1].m_colorMap.Load(L"source/assets/Samurai/Head_BaseColor.dds");
			m[1].m_normalMap.Load(L"source/assets/Samurai/Head_Normal.dds");
			m[1].m_roughnessMap.Load(L"source/assets/Samurai/Head_Roughness.dds");
			m[1].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.01f);

			m[2].m_colorMap.Load(L"source/assets/Samurai/Eye_BaseColor.dds");
			m[2].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, false, false, false, 0.02f, 0.2f);

			m[3].m_colorMap.Load(L"source/assets/Samurai/Helmet_BaseColor.dds");
			m[3].m_normalMap.Load(L"source/assets/Samurai/Helmet_Normal.dds");
			m[3].m_roughnessMap.Load(L"source/assets/Samurai/Helmet_Roughness.dds");
			m[3].m_metalnessMap.Load(L"source/assets/Samurai/Helmet_Metallic.dds");
			m[3].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[4].m_colorMap.Load(L"source/assets/Samurai/Torso_BaseColor.dds");
			m[4].m_normalMap.Load(L"source/assets/Samurai/Torso_Normal.dds");
			m[4].m_roughnessMap.Load(L"source/assets/Samurai/Torso_Roughness.dds");
			m[4].m_metalnessMap.Load(L"source/assets/Samurai/Torso_Metallic.dds");
			m[4].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[5].m_colorMap.Load(L"source/assets/Samurai/Legs_BaseColor.dds");
			m[5].m_normalMap.Load(L"source/assets/Samurai/Legs_Normal.dds");
			m[5].m_roughnessMap.Load(L"source/assets/Samurai/Legs_Roughness.dds");
			m[5].m_metalnessMap.Load(L"source/assets/Samurai/Legs_Metallic.dds");
			m[5].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			m[6].m_colorMap.Load(L"source/assets/Samurai/Hand_BaseColor.dds");
			m[6].m_normalMap.Load(L"source/assets/Samurai/Hand_Normal.dds");
			m[6].m_roughnessMap.Load(L"source/assets/Samurai/Hand_Roughness.dds");
			m[6].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.02f);

			m[7].m_colorMap.Load(L"source/assets/Samurai/Torso_BaseColor.dds");
			m[7].m_normalMap.Load(L"source/assets/Samurai/Torso_Normal.dds");
			m[7].m_roughnessMap.Load(L"source/assets/Samurai/Torso_Roughness.dds");
			m[7].m_metalnessMap.Load(L"source/assets/Samurai/Torso_Metallic.dds");
			m[7].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(true, true, true, false);

			Transform transform = Transform::Identity();
			transform.Translate(Vec3(3.5f, -1.f, -1.f));
			transform.Scale(Vec3(2, 2, 2));
			transform.Rotate(Quaternion(-M_PI_2, transform.top()));

			engine::MeshSystem::instance().addInstance(SamuraiModel, m, OpaqueInstances::Instance(transform));
		}

		//CUBES
		{
			std::vector<OpaqueInstances::Material> brick(1);
			brick[0].m_colorMap.Load(L"source/assets/Brick/Brick_Wall_012_COLOR.dds");
			brick[0].m_normalMap.Load(L"source/assets/Brick/Brick_Wall_012_NORM.dds");
			brick[0].m_roughnessMap.Load(L"source/assets/Brick/Brick_Wall_012_ROUGH.dds");
			brick[0].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, false, 0.02f);

			std::vector<OpaqueInstances::Material> stone(1);
			stone[0].m_colorMap.Load(L"source/assets/Stone/Stone_COLOR.dds");
			stone[0].m_normalMap.Load(L"source/assets/Stone/Stone_NORM.dds");
			stone[0].m_roughnessMap.Load(L"source/assets/Stone/Stone_ROUGH.dds");
			stone[0].m_materialConstantBuffer = OpaqueInstances::MaterialConstantBuffer(false, true, true, true, 0.02f);

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

			transform.SetPosition({ 5,0,0 });
			transform.SetScale({ 1.f,100.f,100.f });
			engine::MeshSystem::instance().addInstance(engine::ModelManager::instance().GetUnitCubeModel(), brick, OpaqueInstances::Instance(transform));
		}
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

		if (need_to_rotate_camera)
			RotateCamera();
		if (need_to_move_camera)
		{
			MoveCamera(delta_time * offset);
			need_to_move_camera = false;
		}
		if (need_to_move_object)
			OnRMouseMove(mouse_x, mouse_y);
	}

	void Application::OnKeyDown(WPARAM key)
	{
		input_state[key] = true;
	}

	void Application::OnKeyUp(WPARAM key)
	{
		input_state[key] = false;
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

	void Application::OnLMouseMove(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		end_rotation.e[0] = x;
		end_rotation.e[1] = y;
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

	void Application::OnRMouseMove(WORD x, WORD y)
	{
		if (need_to_move_object)
		{
			mouse_x = x;
			mouse_y = y;
			float xx = (x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
			float yy = (y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

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