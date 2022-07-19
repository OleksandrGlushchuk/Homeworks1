#include "controller.h"
const float p_near = 0.01f, p_far = 10000.f, fovy = M_PI / 3.f;

namespace engine::windows
{
	void Controller::InitPerFrameBuffer()
	{
		D3D11_BUFFER_DESC perFrameBufferDesc;
		perFrameBufferDesc.ByteWidth = sizeof(PerFrameBuffer);
		perFrameBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		perFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		perFrameBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		perFrameBufferDesc.MiscFlags = 0;
		perFrameBufferDesc.StructureByteStride = 0;

		HRESULT result = engine::s_device->CreateBuffer(&perFrameBufferDesc, nullptr, engine::Globals::instance().m_perFrameBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

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
		engine::Globals::instance().bind();
		scene.Draw();
	}

	void Controller::OnChangeWindowSize()
	{
		wnd.OnResize();

		need_to_rotate = false;
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
		engine::Globals::instance().InitSamplerStates();
		engine::Globals::instance().InitTextures();
		engine::Globals::instance().InitShaders();

		//CUBES
		{
			engine::ShaderManager::instance().GetShaderBlobs("cube", Cube::s_vertexShaderBlob, Cube::s_pixelShaderBlob);
			engine::ShaderManager::instance().GetShaders("cube", Cube::s_vertexShader, Cube::s_pixelShader);
			Cube::CreateVertexBuffer();
			Cube::CreateInputLayout();

			scene.cubes.resize(7);

			scene.cubes[0] = Cube(Vec3(0.3f, 3, 3));
			scene.cubes[0].Translate(Vec3(-1, 1, 3));
			scene.cubes[0].Rotate(Quaternion(M_PI_4, scene.cubes[0].forward()));
			scene.cubes[0].SetTexture("roof");

			scene.cubes[1] = Cube(Vec3(0.3f, 3, 3.01f));
			scene.cubes[1].Translate(Vec3(1, 1, 3));
			scene.cubes[1].Rotate(Quaternion(M_PI_4, -scene.cubes[1].forward()));
			scene.cubes[1].SetTexture("roof");

			scene.cubes[2] = Cube(Vec3(0.5f, 3, 2.9f));
			scene.cubes[2].Translate(Vec3(-1.3f, -1.3, 3.0f));
			scene.cubes[2].SetTexture("brick");

			scene.cubes[3] = Cube(Vec3(0.5f, 3, 2.9f));
			scene.cubes[3].Translate(Vec3(1.3f, -1.3, 3.0f));
			scene.cubes[3].SetTexture("brick");

			scene.cubes[4] = Cube(Vec3(2.9f, 3, 0.48f));
			scene.cubes[4].Translate(Vec3(0.f, -1.31, 4.2f));
			scene.cubes[4].SetTexture("brick");

			scene.cubes[5] = Cube(Vec3(2.9f, 0.29f, 2.85f));
			scene.cubes[5].Translate(Vec3(0.f, -2.65f, 3.0f));
			scene.cubes[5].SetTexture("chess");

			scene.cubes[6] = Cube(0.7f);
			scene.cubes[6].Translate(Vec3(0.6f, -2.0f, 3.5f));
			scene.cubes[6].SetTexture("redstone");
		}

		//SKY
		{
			engine::ShaderManager::instance().GetShaderBlobs("sky", Sky::s_vertexShaderBlob, Sky::s_pixelShaderBlob);
			engine::ShaderManager::instance().GetShaders("sky", Sky::s_vertexShader, Sky::s_pixelShader);
			Sky::CreateVertexBuffer();
			Sky::CreateInputLayout();

			scene.sky.SetTexture("sky");
		}

		/*engine::ShaderManager::instance().GetShaderBlobs("triangle", Triangle<MeshType::TexturedVertex3D>::s_vertexShaderBlob, Triangle<MeshType::TexturedVertex3D>::s_pixelShaderBlob);
		engine::ShaderManager::instance().GetShaders("triangle", Triangle<MeshType::TexturedVertex3D>::s_vertexShader, Triangle<MeshType::TexturedVertex3D>::s_pixelShader);
		scene.triangle = Triangle<MeshType::TexturedVertex3D>({ {-1,-1,1,0,1},{0.f,1,1,0.5f,0},{1,-1,1,1,1} });
		scene.triangle.SetTexture("chess");*/
		
		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		scene.camera = Camera(fovy, aspect, p_near, p_far);
		InitPerFrameBuffer();
	}

	void Controller::ProcessInput()
	{
		Vec3 offset = Vec3(0, 0, 0);
		Angles angle(0, 0, 0);
		//MOVEMENT
		{
			if (input_state['A'])
			{
				offset.e[0] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				scene.need_to_redraw = true;
			}
			if (input_state['D'])
			{
				offset.e[0] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				scene.need_to_redraw = true;
			}
			if (input_state['W'])
			{
				offset.e[2] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				scene.need_to_redraw = true;
			}
			if (input_state['S'])
			{
				offset.e[2] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
				scene.need_to_redraw = true;
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
				scene.need_to_redraw = true;
				offset.e[1] -= camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
			}
			if (input_state['E'])
			{
				scene.need_to_redraw = true;
				offset.e[1] += camera_move_offset_val * ((need_to_speed_up) ? acceleration : 1.f);
			}
		}

		//SAMPLE_FILTRATION
		{
			if (input_state['1'])
			{
				engine::TextureManager::instance().SetGlobalSamplerState("ss_a");
			}
			if (input_state['2'])
			{
				engine::TextureManager::instance().SetGlobalSamplerState("ss_mmmp");
			}
			if (input_state['3'])
			{
				engine::TextureManager::instance().SetGlobalSamplerState("ss_mpmlmp");
			}
			if (input_state['4'])
			{
				engine::TextureManager::instance().SetGlobalSamplerState("ss_mmlmp");
			}
			if (input_state['5'])
			{
				engine::TextureManager::instance().SetGlobalSamplerState("ss_mmml");
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
				Angles rotate_angles(0, 0, 0);
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
		if (scene.need_to_redraw)
		{
			moveCamera(delta_time * offset, angle);
			if (need_to_move_object)
			{
				OnRMouseMove(mouse_x, mouse_y);
			}
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
		need_to_rotate = true;
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
		scene.need_to_redraw = true;
	}

	void Controller::OnLMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_rotate = false;
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
		if (need_to_rotate)
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
			scene.need_to_redraw = true;
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
		if (need_to_rotate)
		{
			scene.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
			scene.need_to_redraw = true;
		}
	}
}