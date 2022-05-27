#include "controller.h"
#include "../math/ObjectMover.h"

void Controller::ProcessInput()
{
	Vec3 offset(0,0,0);
	Angles angle;
	if (GetKeyState('W') < 0)
	{
		scene.need_to_redraw = true;
		offset.e[2] += camera_move_offset_val;
	}
	if (GetKeyState('S') < 0)
	{
		scene.need_to_redraw = true;
		offset.e[2] -= camera_move_offset_val;
	}
	if (GetKeyState('A') < 0)
	{
		scene.need_to_redraw = true;
		offset.e[0] -= camera_move_offset_val;
	}
	if (GetKeyState('D') < 0)
	{
		scene.need_to_redraw = true;
		offset.e[0] += camera_move_offset_val;
	}
	if (GetKeyState(VK_SPACE) < 0)
	{
		scene.need_to_redraw = true;
		offset.e[1] += camera_move_offset_val;
	}
	if (GetKeyState(VK_CONTROL) < 0)
	{
		scene.need_to_redraw = true;
		offset.e[1] -= camera_move_offset_val;
	}
	if (GetKeyState('Y') < 0)
	{
		scene.need_to_redraw = true;
		angle.pitch += camera_angle_offset_val;
	}
	if (GetKeyState('H') < 0)
	{
		scene.need_to_redraw = true;
		angle.pitch -= camera_angle_offset_val;
	}
	if (GetKeyState('G') < 0)
	{
		scene.need_to_redraw = true;
		angle.yaw += camera_angle_offset_val;
	}
	if (GetKeyState('J') < 0)
	{
		scene.need_to_redraw = true;
		angle.yaw -= camera_angle_offset_val;
	}
	if (GetKeyState('Q') < 0)
	{
		scene.need_to_redraw = true;
		angle.roll -= camera_angle_offset_val;
	}
	if (GetKeyState('E') < 0)
	{
		scene.need_to_redraw = true;
		angle.roll += camera_angle_offset_val;
	}
	RotateCamera();
	if (scene.need_to_redraw)
	{
		moveCamera(offset, angle);
		if (need_to_move_object)
		{
			OnRMouseMove(mouse_x, mouse_y);
		}
	}
}

void Controller::moveCamera(const Vec3& offset, const Angles& angles)
{
	scene.camera.addRelativeAngles(angles);
	scene.camera.addRelativeOffset(offset);
	scene.camera.updateMatrices();
}

Vec3 start_rotation(0, 0, 1), end_rotation(0, 0, 1), dir_rotation(0, 0, 1);
void Controller::OnLMouseDown(WORD x, WORD y)
{
	need_to_rotate = false;
	mouse_x = x;
	mouse_y = y;
	start_rotation.e[0] = x;
	start_rotation.e[1] = y;
}

void Controller::OnLMouseMove(WORD x, WORD y)
{
	mouse_x = x;
	mouse_y = y;
	end_rotation.e[0] = x;
	end_rotation.e[1] = y;
	dir_rotation = (start_rotation - end_rotation) / 50.f;
	scene.need_to_redraw = true;
}

void Controller::OnLMouseUp(WORD x, WORD y)
{
	need_to_rotate = true;
	mouse_x = x;
	mouse_y = y;
	end_rotation.e[0] = x;
	end_rotation.e[1] = y;
	dir_rotation = (start_rotation - end_rotation) / 50.f;
	scene.need_to_redraw = true;
}

void Controller::RotateCamera()
{
	if (need_to_rotate)
	{
		scene.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
		if (need_to_move_object)
			OnRMouseMove(mouse_x, mouse_y);
		scene.need_to_redraw = true;
	}
}

void Controller::OnRMouseDown(WORD x, WORD y)
{
	mouse_x = x;
	mouse_y = y;
	nearest_clicked_object.reset();
	need_to_move_object = false;
	float xx = (static_cast<float>(x) + 0.5f) / ((static_cast<float>(wnd.screen.right) - 1.f) / 2.f) - 1;
	float yy = (static_cast<float>(y) + 0.5f) / ((static_cast<float>(wnd.screen.bottom) - 1.f) / (-2.f)) + 1;

	ray_clicked_to_object.origin = Vec3(xx, yy, 1);
	ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv,1);
	ray_clicked_to_object.direction = ray_clicked_to_object.origin - scene.camera.position();
	make_unit_vector(ray_clicked_to_object.direction);

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
	if (need_to_move_object)
	{
		mouse_x = x;
		mouse_y = y;
		float xx = (static_cast<float>(x) + 0.5f) / ((static_cast<float>(wnd.screen.right) - 1.f) / 2.f) - 1;
		float yy = (static_cast<float>(y) + 0.5f) / ((static_cast<float>(wnd.screen.bottom) - 1.f) / (-2.f)) + 1;

		ray_clicked_to_object.origin = Vec3(xx, yy, 1);
		ray_clicked_to_object.origin.mult(scene.camera.m_viewProjInv, 1);
		ray_clicked_to_object.direction = ray_clicked_to_object.origin - scene.camera.position();

		Vec3 mousepos = ray_clicked_to_object.origin;
		ray r = ray_clicked_to_object;
		make_unit_vector(ray_clicked_to_object.direction);


		scene.clicked_plane.normal = -scene.camera.forward();
		scene.clicked_plane.point = distance_object_to_camera * (ray_clicked_to_object.direction) + scene.camera.position();
		new_object_intersection.reset();
		scene.clicked_plane.intersection(new_object_intersection.nearest, r);

		nearest_clicked_object.mover->move(new_object_intersection.nearest.point - nearest_clicked_object.nearest.point_without_translation);

		scene.need_to_redraw = true;
	}
}

void Controller::OnRMouseUp(WORD x, WORD y)
{
	mouse_x = x;
	mouse_y = y;
	need_to_move_object = false;
}

void Controller::OnChangeWindowSize()
{
	wnd.OnResize();
	need_to_rotate = false;
	InitScene();
	scene.Redraw(wnd);
	scene.Draw(wnd);
}

void Controller::InitScene()
{
	//SPHERES INITIALIZING
	{
		scene.sp[0] = Sphere(Vec3(70, 100, 70), 40);
		scene.sp[1] = Sphere(Vec3(-40, 50, 80), 15);
		scene.sp[0].material.albedo = Vec3(0, 0, 0.9f);
		scene.sp[0].material.emmission = Vec3(0, 0, 0.1f);
		scene.sp[0].material.specular = 0.9f;
		scene.sp[0].material.glossiness = 8;

		scene.sp[1].material.albedo = Vec3(0.8, 0.3f, 0);
		scene.sp[1].material.emmission = Vec3(0.08, 0.03f, 0);
		scene.sp[1].material.specular = 0.9f;
		scene.sp[1].material.glossiness = 5;
		scene.sp[0].material.only_emmission = scene.sp[1].material.only_emmission = false;
	}

	//LIGHTS INITIALIZING
	{
		scene.sphere_point_light[0] = Sphere_Point_Light(Vec3(10, 180, 40), 1);
		scene.sphere_point_light[0].light.color = Vec3(255, 255, 255);
		scene.sphere_point_light[0].light.light_radius = 130;

		scene.sphere_point_light[1] = Sphere_Point_Light(Vec3(-20, 79, 150), 1);
		scene.sphere_point_light[1].light.color = Vec3(220, 220, 220);
		scene.sphere_point_light[1].light.light_radius = 110;


		scene.sphere_spot_light[0] = Sphere_Spot_Light(Vec3(50, 10, -100), 1, Vec3(-1, -0.1, 0));
		scene.sphere_spot_light[0].light.color = Vec3(255, 255, 255);
		scene.sphere_spot_light[0].light.cutoff = cosf(M_PI / 6.f);
		scene.sphere_spot_light[0].light.light_radius = 250;


		scene.dir_light[0].color = Vec3(150, 150, 150);
		scene.dir_light[0].direction = Vec3(0, 1, -1);
	}

	//CUBES INITIALIZING
	{
		scene.cube[0] = Cube(Vec3(10, 38, 30));
		scene.cube[0].Rotate(Quaternion(45, scene.cube[0].forward()));
		scene.cube[0].Translate(Vec3(0, 40, 70));
		scene.cube[0].material.albedo = Vec3(0.7f, 0.7f, 0.1f);
		scene.cube[0].material.emmission = Vec3(0.7f, 0.7f, 0.1f);
		scene.cube[0].material.specular = 0.6f;
		scene.cube[0].material.glossiness = 6;
		scene.cube[0].material.only_emmission = false;

		scene.cube[1] = Cube(Vec3(10, 38, 30));
		scene.cube[1].Rotate(Quaternion(-45, scene.cube[0].forward()));
		scene.cube[1].Translate(Vec3(20, 40, 70));
		scene.cube[1].material.albedo = Vec3(0.7f, 0.7f, 0.1f);
		scene.cube[1].material.emmission = Vec3(0.7f, 0.7f, 0.1f);
		scene.cube[1].material.specular = 0.6f;
		scene.cube[1].material.glossiness = 6;
		scene.cube[1].material.only_emmission = false;

		scene.cube[2] = Cube(Vec3(30, 40, 30));
		scene.cube[2].Translate(Vec3(10, 10, 71));
		scene.cube[2].material.albedo = Vec3(0, 0, 0.9f);
		scene.cube[2].material.emmission = Vec3(0, 0, 0.1f);
		scene.cube[2].material.specular = 0.9f;
		scene.cube[2].material.glossiness = 8;
		scene.cube[2].material.only_emmission = false;

		scene.cube[3] = Cube(Vec3(30, 30, 30));
		scene.cube[3].Translate(Vec3(-10, 10, -101));
		scene.cube[3].material.albedo = Vec3(0.9f, 0, 0.1f);
		scene.cube[3].material.emmission = Vec3(0.2f, 0, 0.05f);
		scene.cube[3].material.specular = 0.9f;
		scene.cube[3].material.glossiness = 8;
		scene.cube[3].material.only_emmission = false;
	}

	//FLOOR INITIALIZING
	{
		scene.floor.material.albedo = Vec3(0, 0.5f, 0);
		scene.floor.material.emmission = Vec3(0, 0.05f, 0);
		scene.floor.material.specular = 0.7f;
		scene.floor.material.glossiness = 7;
		scene.floor.material.only_emmission = false;
	}

	static float p_near = 1.f, p_far = 1002.f, fovy = M_PI / 3.f;

	float aspect = float(wnd.screen.right) / wnd.screen.bottom;
	scene.camera = Camera(fovy, aspect, p_near, p_far);
}