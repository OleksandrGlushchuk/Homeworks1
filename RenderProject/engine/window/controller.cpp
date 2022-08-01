#include "controller.h"
const float p_near = 0.01f, p_far = 10000.f, fovy = M_PI / 3.f;

namespace engine::windows
{

	void Controller::UpdatePerFrameBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT result = engine::s_deviceContext->Map(engine::Globals::instance().m_perFrameBuffer.ptr(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ALWAYS_ASSERT(result >= 0 && "Map");

		*(PerFrameBuffer*)(mappedSubresource.pData) = PerFrameBuffer(scene.camera.m_viewProj, scene.camera.BottomLeft, scene.camera.BR_M_BL, scene.camera.TL_M_BL, scene.camera.m_viewProjInv);
		engine::s_deviceContext->Unmap(engine::Globals::instance().m_perFrameBuffer.ptr(), 0);
	}

	void Controller::DrawScene()
	{
		ProcessInput();
		UpdatePerFrameBuffer();
		engine::MeshSystem::instance().updateInstances();
		engine::Globals::instance().Bind();
		engine::MeshSystem::instance().render();
		scene.Draw();
	}

	void Controller::OnChangeWindowSize()
	{
		wnd.OnResize();

		need_to_rotate_camera = false;
		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		scene.camera.updateAspect(aspect);
		scene.camera.updateBasis();
		scene.camera.updateMatrices();

		UpdatePerFrameBuffer();

		wnd.BeginFrame();
		DrawScene();
		wnd.EndFrame();
	}

	void Controller::InitScene()
	{
		//SKY
		{
			scene.sky.Init();
			scene.sky.SetTexture(L"source/textures/skymap.dds");
		}
		//OpaqueInstances
		{
			//KNIGHTS
			{
				Knight::Init();
				scene.knight.resize(2);
				std::vector<OpaqueInstances::Material> m(9);
				m[0].m_texture.Load(L"source/assets/Knight/Fur_BaseColor.dds");
				m[1].m_texture.Load(L"source/assets/Knight/Legs_BaseColor.dds");
				m[2].m_texture.Load(L"source/assets/Knight/Torso_BaseColor.dds");
				m[3].m_texture.Load(L"source/assets/Knight/Head_BaseColor.dds");
				m[4].m_texture.Load(L"source/assets/Knight/Eye_BaseColor.dds");
				m[5].m_texture.Load(L"source/assets/Knight/Helmet_BaseColor.dds");
				m[6].m_texture.Load(L"source/assets/Knight/Skirt_BaseColor.dds");
				m[7].m_texture.Load(L"source/assets/Knight/Cape_BaseColor.dds");
				m[8].m_texture.Load(L"source/assets/Knight/Glove_BaseColor.dds");


				
				engine::MeshSystem::instance().addInstance(Knight::s_model, m, scene.knight[0].ID);
				engine::MeshSystem::instance().Translate(scene.knight[0].ID, Vec3(-1.5f, -0.9f, 0));
				engine::MeshSystem::instance().Rotate(scene.knight[0].ID, Quaternion(M_PI_4, engine::MeshSystem::instance().Get_Forward(scene.knight[0].ID)));
				engine::MeshSystem::instance().Rotate(scene.knight[0].ID, Quaternion(M_PI_2, engine::MeshSystem::instance().Get_Top(scene.knight[0].ID)));

				m[2].m_texture.Load(L"source/assets/Samurai/Torso_BaseColor.dds"); //NEW MATERIAL FOR TORSO MESH AS EXAMPLE
				engine::MeshSystem::instance().addInstance(Knight::s_model, m, scene.knight[1].ID);
				engine::MeshSystem::instance().Scale(scene.knight[1].ID, Vec3(0.5f, 0.5f, 0.5f));
				engine::MeshSystem::instance().Translate(scene.knight[1].ID, Vec3(1.f, 0, 0.6f));
			}

			//SAMURAI
			{
				Samurai::Init();
				scene.samurai.resize(1);
				std::vector<OpaqueInstances::Material> m(8);
				m[0].m_texture.Load(L"source/assets/Samurai/Sword_BaseColor.dds");
				m[1].m_texture.Load(L"source/assets/Samurai/Head_BaseColor.dds");
				m[2].m_texture.Load(L"source/assets/Samurai/Eye_BaseColor.dds");
				m[3].m_texture.Load(L"source/assets/Samurai/Helmet_BaseColor.dds");
				m[4].m_texture.Load(L"source/assets/Knight/Skirt_BaseColor.dds");
				m[5].m_texture.Load(L"source/assets/Samurai/Legs_BaseColor.dds");
				m[6].m_texture.Load(L"source/assets/Samurai/Hand_BaseColor.dds");
				m[7].m_texture.Load(L"source/assets/Samurai/Torso_BaseColor.dds");
				engine::MeshSystem::instance().addInstance(Samurai::s_model, m, scene.samurai[0].ID);
				engine::MeshSystem::instance().Translate(scene.samurai[0].ID, Vec3(3.5f, -0.5f, -1));
				engine::MeshSystem::instance().Scale(scene.samurai[0].ID, Vec3(2.5f, 2.5f, 2.5f));
				engine::MeshSystem::instance().Rotate(scene.samurai[0].ID, Quaternion(-M_PI_2, engine::MeshSystem::instance().Get_Top(scene.samurai[0].ID)));
			}

			//CUBES
			{
				Cube::Init();
				scene.cube.resize(7);

				std::vector<OpaqueInstances::Material> brick(1);
				brick[0].m_texture.Load(L"source/textures/brick.dds");

				std::vector<OpaqueInstances::Material> roof(1);
				roof[0].m_texture.Load(L"source/textures/roof.dds");

				std::vector<OpaqueInstances::Material> redstone(1);
				redstone[0].m_texture.Load(L"source/textures/redstone.dds");

				std::vector<OpaqueInstances::Material> chess(1);
				chess[0].m_texture.Load(L"source/textures/chess.dds");

				engine::MeshSystem::instance().addInstance(Cube::s_model, brick, scene.cube[0].ID);
				engine::MeshSystem::instance().Scale(scene.cube[0].ID, Vec3(0.5f, 3, 3));
				engine::MeshSystem::instance().Translate(scene.cube[0].ID, Vec3(-2, 0, 0));
	
				engine::MeshSystem::instance().addInstance(Cube::s_model, brick, scene.cube[1].ID);
				engine::MeshSystem::instance().Scale(scene.cube[1].ID, Vec3(0.5f, 3, 3));
				engine::MeshSystem::instance().Translate(scene.cube[1].ID, Vec3(2, 0, 0));

				engine::MeshSystem::instance().addInstance(Cube::s_model, brick, scene.cube[2].ID);
				engine::MeshSystem::instance().Scale(scene.cube[2].ID, Vec3(4, 3, 0.5f));
				engine::MeshSystem::instance().Translate(scene.cube[2].ID, Vec3(0, 0, 1.8f));

				engine::MeshSystem::instance().addInstance(Cube::s_model, roof, scene.cube[3].ID);
				engine::MeshSystem::instance().Scale(scene.cube[3].ID, Vec3(0.2f, 4, 3));
				engine::MeshSystem::instance().Translate(scene.cube[3].ID, Vec3(-1.4f, 2, 0.02f));
				engine::MeshSystem::instance().Rotate(scene.cube[3].ID, Quaternion(M_PI_4, Vec3(0,0,1)));

				engine::MeshSystem::instance().addInstance(Cube::s_model, roof, scene.cube[4].ID);
				engine::MeshSystem::instance().Scale(scene.cube[4].ID, Vec3(0.2f, 4, 3));
				engine::MeshSystem::instance().Translate(scene.cube[4].ID, Vec3(1.4f, 2, 0.01f));
				engine::MeshSystem::instance().Rotate(scene.cube[4].ID, Quaternion(-M_PI_4, Vec3(0, 0, 1)));

				engine::MeshSystem::instance().addInstance(Cube::s_model, redstone, scene.cube[5].ID);
				engine::MeshSystem::instance().Translate(scene.cube[5].ID, Vec3(1.f, -0.5f, 1.f));

				engine::MeshSystem::instance().addInstance(Cube::s_model, chess, scene.cube[6].ID);
				engine::MeshSystem::instance().Scale(scene.cube[6].ID, Vec3(4, 0.5f, 3));
				engine::MeshSystem::instance().Translate(scene.cube[6].ID, Vec3(0, -1.3f, 0.1f));

			}
		}

		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		scene.camera = Camera(fovy, aspect, p_near, p_far);
		scene.camera.setWorldOffset(Vec3(0, 0, -5));
	}

	void Controller::ProcessInput()
	{
		Vec3 offset = Vec3(0, 0, 0);
		Angles angle;
		//MOVEMENT
		{
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
			if (input_state[VK_SHIFT])
			{
				need_to_speed_up = true;
			}
			else
			{
				need_to_speed_up = false;
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
				engine::Globals::instance().SetGlobalSamplerState("ss_a");
			}
			if (input_state['2'])
			{
				engine::Globals::instance().SetGlobalSamplerState("ss_mmmp");
			}
			if (input_state['3'])
			{
				engine::Globals::instance().SetGlobalSamplerState("ss_mpmlmp");
			}
			if (input_state['4'])
			{
				engine::Globals::instance().SetGlobalSamplerState("ss_mmlmp");
			}
			if (input_state['5'])
			{
				engine::Globals::instance().SetGlobalSamplerState("ss_mmml");
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
				if (input_state[VK_CONTROL])
				{
					rotate_angles.roll -= object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (input_state[VK_SPACE])
				{
					rotate_angles.roll += object_rotate_angle;
					need_to_rotate_object = true;
				}
				if (need_to_rotate_object)
				{
					rotate_angles.pitch *= delta_time; rotate_angles.roll *= delta_time; rotate_angles.yaw *= delta_time;
					if (need_to_rotate_object_relative_to_camera_axes)
					{
						nearest_clicked_object.rotator->rotate(rotate_angles, scene.camera.right(), scene.camera.top(), scene.camera.forward());
					}
					else
					{
						nearest_clicked_object.rotator->rotate(rotate_angles, nearest_clicked_object.rotator->get_right(), nearest_clicked_object.rotator->get_top(), nearest_clicked_object.rotator->get_forward());
					}
					need_to_rotate_object = false;
				}
			}
		}

		RotateCamera();
		if (need_to_move_camera)
		{
			moveCamera(delta_time * offset, angle);
			if (need_to_move_object)
			{
				OnRMouseMove(mouse_x, mouse_y);
			}
			need_to_move_camera = false;
		}
	}

	void Controller::OnKeyDown(WPARAM key)
	{
		input_state[key] = true;
	}

	void Controller::OnKeyUp(WPARAM key)
	{
		input_state[key] = false;
	}

	void Controller::moveCamera(const Vec3& offset, const Angles& angles)
	{
		scene.camera.addRelativeAngles(angles);
		scene.camera.addRelativeOffset(offset);
		scene.camera.updateMatrices();
	}

	void Controller::OnLMouseDown(WORD x, WORD y)
	{
		need_to_rotate_camera = true;
		mouse_x = x;
		mouse_y = y;
		start_rotation.e[0] = end_rotation.e[0] = x;
		start_rotation.e[1] = end_rotation.e[1] = y;
		dir_rotation = Vec3(0, 0, 1);
	}

	void Controller::OnLMouseMove(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		end_rotation.e[0] = x;
		end_rotation.e[1] = y;
		dir_rotation = delta_time * (start_rotation - end_rotation) * 2.f * M_PI / wnd.screen.right;
	}

	void Controller::OnLMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_rotate_camera = false;
	}

	void Controller::OnRMouseDown(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		nearest_clicked_object.reset();
		need_to_move_object = false;

		float xx = (x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
		float yy = (y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

		ray_clicked_to_object.origin = Vec3(xx, yy, 1);

		float w;
		ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv, 1, &w);
		ray_clicked_to_object.origin /= w;

		ray_clicked_to_object.direction = ray_clicked_to_object.origin - scene.camera.position();
		ray_clicked_to_object.direction.normalize();
		

		if (scene.findIntersection(ray_clicked_to_object, nearest_clicked_object))
		{
			if (nearest_clicked_object.mover != nullptr)
			{
				distance_object_to_camera = (nearest_clicked_object.nearest.point - scene.camera.position()).length();
				need_to_move_object = true;
			}
		}
	}

	void Controller::OnRMouseMove(WORD x, WORD y)
	{
		if (need_to_rotate_camera)
			OnLMouseMove(x, y);
		if (need_to_move_object)
		{
			mouse_x = x;
			mouse_y = y;
			float xx = (x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
			float yy = (y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

			ray_clicked_to_object.origin = Vec3(xx, yy, 1);

			float w;
			ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv, 1, &w);
			ray_clicked_to_object.origin /= w;

			ray_clicked_to_object.direction = ray_clicked_to_object.origin - scene.camera.position();
			ray_clicked_to_object.direction.normalize();

			Vec3 new_position = ray_clicked_to_object.direction * distance_object_to_camera + scene.camera.position();
			nearest_clicked_object.mover->move(new_position - nearest_clicked_object.nearest.point);
			nearest_clicked_object.nearest.point = new_position;
		}
	}

	void Controller::OnRMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_move_object = false;
	}

	void Controller::OnMouseWheel(short wheel_data)
	{
		short count = wheel_data / WHEEL_DELTA;
		if (count < 0)
			camera_move_offset_val /= abs(count) * 1.1f;
		else
			camera_move_offset_val *= abs(count) * 1.1f;
	}

	void Controller::RotateCamera()
	{
		if (need_to_rotate_camera)
		{
			scene.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
			need_to_move_camera = true;
		}
	}
}