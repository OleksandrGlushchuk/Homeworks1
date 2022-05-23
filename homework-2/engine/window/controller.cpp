#include "controller.h"

void Controller::ProcessInput()
{
	Vec3 offset(0,0,0);
	Angles angle;
	static float off = 3;
	static float angle_val = 3;
	if (GetKeyState('W') < 0)
	{
		render.need_to_redraw = true;
		offset.e[2] += off;
	}
	if (GetKeyState('S') < 0)
	{
		render.need_to_redraw = true;
		offset.e[2] -= off;
	}
	if (GetKeyState('A') < 0)
	{
		render.need_to_redraw = true;
		offset.e[0] -= off;
	}
	if (GetKeyState('D') < 0)
	{
		render.need_to_redraw = true;
		offset.e[0] += off;
	}
	if (GetKeyState(VK_SPACE) < 0)
	{
		render.need_to_redraw = true;
		offset.e[1] += off;
	}
	if (GetKeyState(VK_CONTROL) < 0)
	{
		render.need_to_redraw = true;
		offset.e[1] -= off;
	}
	if (GetKeyState('Y') < 0)
	{
		render.need_to_redraw = true;
		angle.pitch += angle_val;
	}
	if (GetKeyState('H') < 0)
	{
		render.need_to_redraw = true;
		angle.pitch -= angle_val;
	}
	if (GetKeyState('G') < 0)
	{
		render.need_to_redraw = true;
		angle.yaw += angle_val;
	}
	if (GetKeyState('J') < 0)
	{
		render.need_to_redraw = true;
		angle.yaw -= angle_val;
	}
	if (GetKeyState('Q') < 0)
	{
		render.need_to_redraw = true;
		angle.roll -= angle_val;
	}
	if (GetKeyState('E') < 0)
	{
		render.need_to_redraw = true;
		angle.roll += angle_val;
	}

	if (render.need_to_redraw)
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
	render.camera.addRelativeAngles(angles);
	render.camera.addRelativeOffset(offset);
	render.camera.updateMatrices();
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
	dir_rotation = (start_rotation - end_rotation) / 100.f;
	render.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
	render.need_to_redraw = true;
}

void Controller::OnLMouseUp(WORD x, WORD y)
{
	need_to_rotate = true;
	mouse_x = x;
	mouse_y = y;
	end_rotation.e[0] = x;
	end_rotation.e[1] = y;
	dir_rotation = (start_rotation - end_rotation) / 100.f;
	render.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
	render.need_to_redraw = true;
}

void Controller::Rotate()
{
	if (need_to_rotate)
	{
		render.camera.addRelativeAngles(Angles(0, dir_rotation.e[1], dir_rotation.e[0]));
		if (need_to_move_object)
			OnRMouseMove(mouse_x, mouse_y);
		render.need_to_redraw = true;
	}
}

ray ray_to_object;
Intersection nearest;
Floor fl;
void Controller::OnRMouseDown(WORD x, WORD y)
{
	mouse_x = x;
	mouse_y = y;
	nearest.reset();
	need_to_move_object = false;
	float xx = x / ((wnd.screen.right - 1) / 2.f) - 1;
	float yy = y / ((wnd.screen.bottom - 1) / (-2.f)) + 1;
	ray_to_object.origin = Vec3(xx, yy, 1);
	ray_to_object.origin.mult(render.camera.m_viewProjInv,1);
	ray_to_object.direction = ray_to_object.origin - render.camera.position();
	ray_to_object.direction.make_unit_vector();
	if (render.FindIntersection(ray_to_object, nearest))
	{
		if (nearest.cube != nullptr || nearest.sphere != nullptr)
		{
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
		fl.normal = -render.camera.forward();
		fl.normal.make_unit_vector();
		fl.point = nearest.point;
		float xx = x / ((wnd.screen.right - 1) / 2.f) - 1;
		float yy = y / ((wnd.screen.bottom - 1) / (-2.f)) + 1;
		Vec3 mousepos(xx, yy, 1);
		mousepos.mult(render.camera.m_viewProjInv, 1);
		ray r;
		r.origin = render.camera.position();
		r.direction = mousepos - r.origin;
		r.direction.make_unit_vector();
		Intersection fl_nearest;
		fl_nearest.reset();
		fl.intersection(fl_nearest, r);

		if (nearest.cube != nullptr)
		{
			Cube* cube = const_cast<Cube*>(nearest.cube);
			cube->SetPosition(fl_nearest.point.x() - nearest.point_model.x(), fl_nearest.point.y() - nearest.point_model.y(), fl_nearest.point.z() - nearest.point_model.z());
		}
		if (nearest.sphere != nullptr)
		{
			Sphere* sphere = const_cast<Sphere*>(nearest.sphere);
			sphere->center = Vec3(fl_nearest.point.x() - nearest.point_model.x(), fl_nearest.point.y() - nearest.point_model.y(), fl_nearest.point.z() - nearest.point_model.z());
		}
		render.need_to_redraw = true;
	}
}

void Controller::OnRMouseUp(WORD x, WORD y)
{
	mouse_x = x;
	mouse_y = y;
	need_to_move_object = false;
	nearest.cube = nullptr;
	nearest.sphere = nullptr;
}

void Controller::OnChangeWindowSize()
{
	need_to_rotate = false;
	render.Init(wnd);
	render.Redraw(wnd);
	render.Draw(wnd);
}