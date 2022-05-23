#include "render.h"
#include "../math/Matr.h"
#include "../math/camera.h"
#include "../math/Intersection.h"
#define  _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

float p_near = 1.f, p_far = 1002.f, fovy = M_PI/3.f;

void Render::Init(Window& wnd)
{
	sp[0] = Sphere(Vec3(70, 100, 70), 40);
	sp[1] = Sphere(Vec3(-40, 50, 80), 15);
	sp[0].mat.albedo = Vec3(0, 0, 0.9f);
	sp[0].mat.emmission = Vec3(0, 0, 0.1f);
	sp[0].mat.specular = 0.9f;
	sp[0].mat.glossiness = 8;

	sp[1].mat.albedo = Vec3(0.8, 0.3f, 0);
	sp[1].mat.emmission = Vec3(0.08, 0.03f, 0);
	sp[1].mat.specular = 0.9f;
	sp[1].mat.glossiness = 5;
	sp[0].mat.only_emmission = sp[1].mat.only_emmission = false;

	light[0].color = Vec3(255, 255, 255);
	light[0].R = 150;
	light[0].pos = Vec3(10, 180, 40);

	light[1].color = Vec3(255, 255, 255);
	light[1].R = 150;
	light[1].pos = Vec3(-80, 79, -40);

	light_sphere[0] = Sphere(light[0].pos, 1);
	light_sphere[0].mat.emmission = Vec3(255, 255, 255);
	light_sphere[0].mat.only_emmission = true;

	light_sphere[1] = Sphere(light[1].pos, 1);
	light_sphere[1].mat.emmission = Vec3(255, 255, 255);
	light_sphere[1].mat.only_emmission = true;

	dirlight.color = Vec3(150, 150, 150);
	dirlight.direction = Vec3(0, 1, -1);

	spotlight.color = Vec3(255, 255, 255);
	spotlight.cutoff = cosf(M_PI / 6.f);
	spotlight.R = 250;
	spotlight.pos = Vec3(-200, 70.f, 100);
	spotlight.direction = Vec3(1, 0, 0);

	light_sphere[2] = Sphere(spotlight.pos, 1);
	light_sphere[2].mat.emmission = Vec3(255, 255, 255);
	light_sphere[2].mat.only_emmission = true;

	cube[0] = Cube(10);
	cube[0].Translate(20, 20, 70);
	cube[0].mat.albedo = Vec3(0, 0, 0.9f);
	cube[0].mat.emmission = Vec3(0, 0, 0.1f);
	cube[0].mat.specular = 0.9f;
	cube[0].mat.glossiness = 8;
	cube[0].mat.only_emmission = false;

	cube[1] = Cube(30);
	cube[1].Translate(-60, 30, -40);
	cube[1].mat.albedo = Vec3(0.9f, 0, 0);
	cube[1].mat.emmission = Vec3(0.1f, 0, 0);
	cube[1].mat.specular = 0.9f;
	cube[1].mat.glossiness = 8;
	cube[1].mat.only_emmission = false;

	floor.mat.albedo = Vec3(0, 0.5f, 0);
	floor.mat.emmission = Vec3(0, 0.05f, 0);
	floor.mat.specular = 0.9f;
	floor.mat.glossiness = 10;
	floor.mat.only_emmission = false;

	float aspect = float(wnd.screen.right) / wnd.screen.bottom;
	camera.setPerspective(fovy, aspect, p_near, p_far);
	camera.setWorldOffset(Vec3(0, 0, 0));
	camera.setWorldAngles(Angles(0, 0, 0));
	camera.updateBasis();
	camera.updateMatrices();
}

bool Render::FindIntersection(const ray& r, Intersection& nearest)
{
	nearest.reset();
	bool found_intersection = false;

	for (int i = 0; i < sp.size(); i++)
	{
		found_intersection |= sp[i].intersection(nearest, r);
	}
	for (int i = 0; i < light_sphere.size(); i++)
	{
		found_intersection |= light_sphere[i].intersection(nearest, r);
	}
	for (int i = 0; i < cube.size(); i++)
	{
		found_intersection |= cube[i].intersection(nearest, r);
	}

	found_intersection |= floor.intersection(nearest, r);

	return found_intersection;
}

void Render::DrawNearest(Intersection& nearest, RGBQUAD& pixel)
{

	if (nearest.mat.only_emmission)
	{
		pixel.rgbRed = nearest.mat.emmission.r();
		pixel.rgbGreen = nearest.mat.emmission.g();
		pixel.rgbBlue = nearest.mat.emmission.b();
		return;
	}
	Vec3 ambient(20,20,20);
	Vec3 Lo;
	int color_value;
	Vec3 sum(0,0,0);
	ray back_ray;
	Intersection back_nearest;
	back_ray.origin = nearest.point +  nearest.normal;
	pixel.rgbRed = 0;
	pixel.rgbGreen =0;
	pixel.rgbBlue = 0;
	/////////////////////////////////////Point_Light/////////////////////////////////////////////////
	for (int i = 0; i < light.size(); i++)
	{
		back_ray.direction = light[i].pos - back_ray.origin;
		back_ray.direction.make_unit_vector();
		back_nearest.reset();
		bool found_intersection = false;
		for (int j = 0; j < sp.size(); j++)
		{
			found_intersection |= sp[j].intersection(back_nearest, back_ray);
		}
		for (int j = 0; j < cube.size(); j++)
		{
			found_intersection |= cube[j].intersection(back_nearest, back_ray);
		}
		found_intersection |= floor.intersection(back_nearest, back_ray);
		if (found_intersection && (back_nearest.point-nearest.point).length() < (light[i].pos - nearest.point).length())
		{
			//if there is another object between the object and the light source
			continue;
		}
		sum += CalculatePointLight(light[i], camera, nearest.point, nearest.normal, nearest.mat);
	}

	/////////////////////////////////////Diraction_Light/////////////////////////////////////////////////
	dirlight.direction.make_unit_vector();
	sum += CalculateDirectionalLight(dirlight, camera, nearest.point, nearest.normal, nearest.mat);

	/////////////////////////////////////Spot_Light/////////////////////////////////////////////////
	Vec3 LightToPixel(nearest.point - spotlight.pos);
	LightToPixel.make_unit_vector();
	spotlight.direction.make_unit_vector();
	float SpotFactor = dot(LightToPixel, spotlight.direction);
	if (SpotFactor >= spotlight.cutoff)
	{
		back_ray.direction = spotlight.pos - back_ray.origin;
		back_ray.direction.make_unit_vector();
		back_nearest.reset();
		bool found_intersection = false;
		for (int i = 0; i < sp.size(); i++)
		{
			found_intersection |= sp[i].intersection(back_nearest, back_ray);
		}
		for (int i = 0; i < cube.size(); i++)
		{
			found_intersection |= cube[i].intersection(back_nearest, back_ray);
		}
		found_intersection |= floor.intersection(back_nearest, back_ray);
		if (found_intersection && (back_nearest.point - nearest.point).length() < (spotlight.pos - nearest.point).length())
		{
			//if there is another object between the object and the light source
			
			goto label;
		}
		sum += CalculatePointLight(spotlight, camera, nearest.point, nearest.normal, nearest.mat);
		
	}

	label:
	Lo = nearest.mat.emmission + ambient * nearest.mat.albedo + sum;

	if (Lo.r() > 255) Lo.e[0] = 255;
	if (Lo.g() > 255) Lo.e[1] = 255;
	if (Lo.b() > 255) Lo.e[2] = 255;
	pixel.rgbRed = Lo.r();
	pixel.rgbGreen = Lo.g();
	pixel.rgbBlue = Lo.b();
}

void Render::Redraw(Window& wnd)
{
	ray r(Vec3(0,0,0), Vec3(0, 0, 1));
	camera.updateMatrices();

	int pixel_index=0;
	int size = wnd.screen.right * wnd.screen.bottom;
	Intersection nearest;
	for (int y = 0; y < wnd.screen.bottom; y++)
	{
		for (int x = 0; x < wnd.screen.right; x++)
		{
			pixel_index = size - wnd.screen.right - wnd.screen.right * y + x;
			r.origin[0] = x / ((wnd.screen.right - 1) / 2.f) - 1;
			r.origin[1] = y / ((wnd.screen.bottom - 1) / (-2.f)) + 1;
			r.origin[2] = 1;

			r.origin.mult(camera.m_viewProjInv,1);
			r.direction = r.origin - camera.position();
			r.direction.make_unit_vector();

			if (FindIntersection(r,nearest))
			{
				DrawNearest(nearest, wnd.image[pixel_index]);
			}
			else
			{
				wnd.image[pixel_index].rgbRed = 60;
				wnd.image[pixel_index].rgbBlue = 170;
				wnd.image[pixel_index].rgbGreen = 60;
			}
		}
	}
}

void Render::Draw(Window& wnd)
{
	if (need_to_redraw)
	{
		need_to_redraw = false;
		Redraw(wnd);
	}
	SetDIBitsToDevice(wnd.device_context, 0, 0, wnd.screen.right, wnd.screen.bottom, 
		0, 0, 0, wnd.screen.bottom, &wnd.image[0], &wnd.bitmap_info, DIB_RGB_COLORS);
}
