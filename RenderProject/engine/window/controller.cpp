#include "controller.h"
const float p_near = 1.f, p_far = 1002.f, fovy = M_PI / 3.f;

void Controller::DrawScene()
{
	ProcessInput();
	scene.Draw(wnd);
}

void Controller::ProcessInput()
{
	Vec3 offset = Vec3(0, 0, 0);
	Angles angle(0,0,0);
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

	//LIGHTING
	{
		if (input_state[VK_OEM_PLUS])
		{
			scene.need_to_redraw = true;
			scene.EV100 += 1.f * delta_time;
		}
		if (input_state[VK_OEM_MINUS])
		{
			scene.need_to_redraw = true;
			scene.EV100 -= 1.f * delta_time;
		}
		if (input_state['R'])
		{
			if (!scene.smooth_reflection)
			{
				scene.need_to_redraw = true;
				scene.smooth_reflection = true;
				input_state['R'] = false;
			}
			else
			{
				scene.need_to_redraw = true;
				scene.smooth_reflection = false;
				input_state['R'] = false;
			}
		}
		if (input_state['G'])
		{
			if (!scene.global_illumination)
			{
				scene.need_to_redraw = true;
				scene.global_illumination = true;
				input_state['G'] = false;
			}
			else
			{
				scene.need_to_redraw = true;
				scene.global_illumination = false;
				input_state['G'] = false;
			}
		}
	}

	//IMAGE COMPRESSION
	{
		if (input_state['1'])
		{
			scene.need_to_redraw = true;
			image_compression = 1u;
			OnChangeWindowSize();
			input_state['1'] = false;
		}
		if (input_state['2'])
		{
			scene.need_to_redraw = true;
			image_compression = 2u;
			OnChangeWindowSize();
			input_state['2'] = false;
		}
		if (input_state['3'])
		{
			scene.need_to_redraw = true;
			image_compression = 3u;
			OnChangeWindowSize();
			input_state['3'] = false;
		}
		if (input_state['4'])
		{
			scene.need_to_redraw = true;
			image_compression = 4u;
			OnChangeWindowSize();
			input_state['4'] = false;
		}
	}


	RotateCamera();
	if (scene.need_to_redraw)
	{
		moveCamera(offset*delta_time, angle);
		if (need_to_move_object)
		{
			OnRMouseMove(mouse_x, mouse_y);
		}
	}
}

void Controller::OnKeyDown(WPARAM key)
{
	input_state[key] = true;
	if(key != 'G')
		scene.global_illumination = false;
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
	scene.global_illumination = false;
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

void Controller::RotateCamera()
{
	if (need_to_rotate)
	{
		scene.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
		scene.need_to_redraw = true;
	}
}

void Controller::OnRMouseDown(WORD x, WORD y)
{
	scene.global_illumination = false;
	mouse_x = x;
	mouse_y = y;
	nearest_clicked_object.reset();
	need_to_move_object = false;
	float xx = (x + 0.5f) / ((wnd.screen.right) / 2.f) - 1.f;
	float yy = (y + 0.5f) / ((wnd.screen.bottom) / (-2.f)) + 1.f;

	ray_clicked_to_object.origin = Vec3(xx, yy, 1);
	ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv, 1);
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
		ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv, 1);
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

void Controller::OnChangeWindowSize()
{
	wnd.OnResize(image_compression);
	need_to_rotate = false;
	float aspect = float(wnd.screen.right) / wnd.screen.bottom;
	scene.camera.updateAspect(aspect);
	scene.camera.updateBasis();
	scene.camera.updateMatrices();
	scene.Redraw(wnd);
	scene.Draw(wnd);
}

void Controller::InitScene()
{
	//SPHERES INITIALIZING
	{
		/*int col = scene.sp.size() / 5;
		int row = scene.sp.size() / col;
		int sp_index;
		float radius = 15;
		float rough = 0.001f;
		float metal = 1.f / row;
		Vec3 f0(0.02f, 0.02f, 0.02f);
		Vec3 albedo(1.0f, 0.843f, 0.0f);
		for (int z = 0; z < row; z++)
		{
			for (int x = 0; x < col; ++x)
			{
				sp_index = z * col + x;
				scene.sp[sp_index] = Sphere(Vec3(-100+x*2.5f*radius, 50, 170 - z * 2.5f * radius), radius);
				scene.sp[sp_index].material.albedo = albedo;
				scene.sp[sp_index].material.emmission = Vec3(0.f, 0.f, 0.f);
				scene.sp[sp_index].material.metallness = metal + z * metal;
				scene.sp[sp_index].material.roughness = rough + (1.f - rough) * float(x)/(col-1);
				scene.sp[sp_index].material.F0 = Vec3::lerp(f0, albedo*0.1f, scene.sp[sp_index].material.metallness);

			}
		}*/
		scene.sp[0] = Sphere(Vec3(35, 35, -10), 7);
		scene.sp[0].material.albedo = Vec3(0.9f, 0.3f, 0.9f);
		scene.sp[0].material.emmission = Vec3(0.f, 0.f, 0.f);
		scene.sp[0].material.metallness = 0.3f;
		scene.sp[0].material.roughness = 0.3f;
		scene.sp[0].material.F0 = Vec3(0.02f, 0.02f, 0.02f);
	}

	//LIGHTS INITIALIZING
	{
		/*scene.sphere_point_light[0] = Sphere_Point_Light(Vec3(10, 180, 60), 15,Vec3(2000.f,2000.f,2000.f));

		scene.sphere_point_light[1] = Sphere_Point_Light(Vec3(300, 89, -80), 10, Vec3(2000.f, 2000.f, 2000.f));*/

		scene.sphere_spot_light[0] = Sphere_Spot_Light(Vec3(10, 25, -50), 8, Vec3(-1, 1, 1), Vec3(70000.f, 70000.0f, 70000.0f));
		scene.sphere_spot_light[0].light.outerCutoff = cosf(M_PI / 3.f);
		scene.sphere_spot_light[0].light.innerCutoff = cosf(M_PI / 6.f);


		/*scene.dir_light[0].radiance = Vec3(3.2f, 3.2f, 3.2f);
		scene.dir_light[0].direction = Vec3(0, 1, -1);
		scene.dir_light[0].solid_angle = M_PI;*/
	}

	//CUBES INITIALIZING
	{
		scene.cube[0] = Cube(Vec3(10, 38, 30));
		scene.cube[0].Rotate(Quaternion(M_PI_4, scene.cube[0].forward()));
		scene.cube[0].Translate(Vec3(40, 40, -70));
		scene.cube[0].material.albedo = Vec3(0.9f, 0.9f, 0.1f);
		scene.cube[0].material.emmission = Vec3(0.0f, 0.0f, 0.0f);

		scene.cube[0].material.roughness = 0.35f;
		scene.cube[0].material.metallness = 0.2f;
		scene.cube[0].material.F0 = Vec3(0.02f, 0.02f, 0.02f);



		scene.cube[1] = Cube(Vec3(10, 38, 30));
		scene.cube[1].Rotate(Quaternion(-M_PI_4, scene.cube[1].forward()));
		scene.cube[1].Translate(Vec3(60, 40, 0));
		scene.cube[1].material.albedo = Vec3(0.9f, 0.9f, 0.1f);
		scene.cube[1].material.emmission = Vec3(0.0f, 0.0f, 0.0f);

		scene.cube[1].material.roughness = 0.35f;
		scene.cube[1].material.metallness = 0.2f;
		scene.cube[1].material.F0 = Vec3(0.02f, 0.02f, 0.02f);



		scene.cube[2] = Cube(Vec3(30, 40, 30));
		scene.cube[2].Translate(Vec3(60, 10, 1));
		scene.cube[2].material.albedo = Vec3(0.05f, 0.05f, 0.9f);
		scene.cube[2].material.emmission = Vec3(0, 0, 0);
		
		scene.cube[2].material.roughness = 0.9f;
		scene.cube[2].material.metallness = 0.2f;
		scene.cube[2].material.F0 = Vec3(0.01f, 0.01f, 0.01f);



		scene.cube[3] = Cube(Vec3(30, 30, 30));
		scene.cube[3].Translate(Vec3(33, 10, -10));
		scene.cube[3].material.albedo = Vec3(0.9f, 0.05f, 0.1f);
		scene.cube[3].material.emmission = Vec3(0, 0, 0);
		
		scene.cube[3].material.roughness = 0.88f;
		scene.cube[3].material.metallness = 0.1f;
		scene.cube[3].material.F0 = Vec3(0.02f, 0.02f, 0.02f);



		scene.cube[4] = Cube(Vec3(20, 100, 200));
		scene.cube[4].Translate(Vec3(-60, 40, 0));
		scene.cube[4].material.albedo = Vec3(0.9f, 0.1f, 0.1f);
		scene.cube[4].material.emmission = Vec3(0, 0, 0);
		scene.cube[4].material.F0 = Vec3(0.001f, 0.001f, 0.001f);
		scene.cube[4].material.metallness = 0.2f;
		scene.cube[4].material.roughness = 0.9f;

		scene.cube[5] = Cube(Vec3(200, 20, 200));
		scene.cube[5].Translate(Vec3(0, 80, 0));
		scene.cube[5].material.albedo = Vec3(0.1f, 0.1f, 0.9f);
		scene.cube[5].material.emmission = Vec3(0, 0, 0);
		scene.cube[5].material.F0 = Vec3(0.001f, 0.001f, 0.001f);
		scene.cube[5].material.metallness = 0.2f;
		scene.cube[5].material.roughness = 0.9f;

		scene.cube[6] = Cube(Vec3(200, 100, 20));
		scene.cube[6].Translate(Vec3(0, 40, 60));
		scene.cube[6].material.albedo = Vec3(0.1f, 0.9f, 0.0f);
		scene.cube[6].material.emmission = Vec3(0, 0, 0);
		scene.cube[6].material.F0 = Vec3(0.001f, 0.001f, 0.001f);
		scene.cube[6].material.metallness = 0.2f;
		scene.cube[6].material.roughness = 0.9f;

	}

	//FLOOR INITIALIZING
	{
		scene.floor.material.albedo = Vec3(0.1f, 0.1f, 0.1f);
		scene.floor.material.emmission = Vec3(0, 0, 0);
		scene.floor.material.roughness = 0.8f;
		scene.floor.material.metallness = 0.1f;
		scene.floor.material.F0 = Vec3(0.01f, 0.01f, 0.01f);
	}

	float aspect = float(wnd.screen.right) / wnd.screen.bottom;
	scene.camera = Camera(fovy, aspect, p_near, p_far);
	scene.need_to_redraw = true;
}