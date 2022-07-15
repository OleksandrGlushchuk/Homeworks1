#include "controller.h"
const float p_near = 0.01f, p_far = 10000.f, fovy = M_PI / 3.f;

namespace engine::windows
{
	void ControllerD3D::InitCameraBuffer()
	{
		D3D11_BUFFER_DESC cameraBufferDesc;
		cameraBufferDesc.ByteWidth = sizeof(PerFrameBuffer);
		cameraBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		cameraBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		cameraBufferDesc.MiscFlags = 0;
		cameraBufferDesc.StructureByteStride = 0;

		HRESULT result = engine::s_device->CreateBuffer(&cameraBufferDesc, nullptr, m_perFrameBuffer.reset());
		ALWAYS_ASSERT(result >= 0 && "CreateBuffer");
	}

	void ControllerD3D::UpdateCameraBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT result = engine::s_deviceContext->Map(m_perFrameBuffer.ptr(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ALWAYS_ASSERT(result >= 0 && "Map");

		*(PerFrameBuffer*)(mappedSubresource.pData) = PerFrameBuffer(scene.camera.m_viewProj, scene.camera.BottomLeft, scene.camera.BR_M_BL, scene.camera.TL_M_BL, scene.camera.m_viewProjInv);
		engine::s_deviceContext->Unmap(m_perFrameBuffer.ptr(), 0);
	}

	void ControllerD3D::DrawScene()
	{
		ProcessInput();
		engine::s_deviceContext->VSSetConstantBuffers(0, 1, &m_perFrameBuffer.ptr());
		scene.Draw();
	}

	void ControllerD3D::OnChangeWindowSize()
	{
		wnd.OnResize();

		need_to_rotate = false;
		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		scene.camera.updateAspect(aspect);
		scene.camera.updateBasis();
		scene.camera.updateMatrices();

		UpdateCameraBuffer();

		wnd.BeginFrame();
		DrawScene();
		wnd.EndFrame();
	}

	void ControllerD3D::InitSamplerStates()
	{
		{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MipLODBias = 0;

		engine::TextureManager::instance().InitSamplerState(samplerDesc, "ss_a");
		engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_a");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;
			engine::TextureManager::instance().InitSamplerState(samplerDesc, "ss_mmmp");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;
			engine::TextureManager::instance().InitSamplerState(samplerDesc, "ss_mpmlmp");
		}

		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;
			engine::TextureManager::instance().InitSamplerState(samplerDesc, "ss_mmlmp");
		}
		
		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MipLODBias = 0;
			engine::TextureManager::instance().InitSamplerState(samplerDesc, "ss_mmml");
		}
	}

	void ControllerD3D::InitTextures()
	{
		engine::TextureManager::instance().InitTexture(L"d3dobjects/textures/brick.dds", "brick");
		engine::TextureManager::instance().InitTexture(L"d3dobjects/textures/skymap.dds", "sky");
		engine::TextureManager::instance().InitTexture(L"d3dobjects/textures/chess.dds", "chess");
		engine::TextureManager::instance().InitTexture(L"d3dobjects/textures/roof.dds", "roof");
		engine::TextureManager::instance().InitTexture(L"d3dobjects/textures/redstone.dds", "redstone");
	}

	void ControllerD3D::InitScene()
	{
		InitSamplerStates();
		InitTextures();
		scene.cubes.resize(7);

		Cube::CreateMesh();

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

		scene.sky.Initialize();

		float aspect = float(wnd.screen.right) / wnd.screen.bottom;
		scene.camera = Camera(fovy, aspect, p_near, p_far);
		InitCameraBuffer();
		UpdateCameraBuffer();
	}

	void ControllerD3D::ProcessInput()
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
				engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_a");
			}
			if (input_state['2'])
			{
				engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_mmmp");
			}
			if (input_state['3'])
			{
				engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_mpmlmp");
			}
			if (input_state['4'])
			{
				engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_mmlmp");
			}
			if (input_state['5'])
			{
				engine::TextureManager::instance().SetGlobalSamplerStateKey("ss_mmml");
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
			moveCamera(/*delta_time * */offset, angle);
			UpdateCameraBuffer();
			if (need_to_move_object)
			{
				OnRMouseMove(mouse_x, mouse_y);
			}
		}
	}

	void ControllerD3D::OnKeyDown(WPARAM key)
	{
		input_state[key] = true;
	}

	void ControllerD3D::OnKeyUp(WPARAM key)
	{
		input_state[key] = false;
	}

	void ControllerD3D::moveCamera(const Vec3& offset, const Angles& angles)
	{
		scene.camera.addRelativeAngles(angles);
		scene.camera.addRelativeOffset(offset);
		scene.camera.updateMatrices();
	}

	void ControllerD3D::OnLMouseDown(WORD x, WORD y)
	{
		need_to_rotate = true;
		mouse_x = x;
		mouse_y = y;
		start_rotation.e[0] = end_rotation.e[0] = x;
		start_rotation.e[1] = end_rotation.e[1] = y;
		dir_rotation = Vec3(0, 0, 1);
	}

	void ControllerD3D::OnLMouseMove(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		end_rotation.e[0] = x;
		end_rotation.e[1] = y;
		dir_rotation = /*delta_time **/ 0.02f * (start_rotation - end_rotation) * 2.f * M_PI / wnd.screen.right;
		scene.need_to_redraw = true;
	}

	void ControllerD3D::OnLMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_rotate = false;
	}

	void ControllerD3D::OnRMouseDown(WORD x, WORD y)
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

	void ControllerD3D::OnRMouseMove(WORD x, WORD y)
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

	void ControllerD3D::OnRMouseUp(WORD x, WORD y)
	{
		mouse_x = x;
		mouse_y = y;
		need_to_move_object = false;
	}

	void ControllerD3D::OnMouseWheel(short wheel_data)
	{
		short count = wheel_data / WHEEL_DELTA;
		if (count < 0)
			camera_move_offset_val /= abs(count) * 1.1f;
		else
			camera_move_offset_val *= abs(count) * 1.1f;
	}

	void ControllerD3D::RotateCamera()
	{
		if (need_to_rotate)
		{
			scene.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
			scene.need_to_redraw = true;
		}
	}
}