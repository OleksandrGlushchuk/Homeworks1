#include "Scene.h"
#include "../math/Matr.h"
#define  _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

void Scene::findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
{
	outNearest.reset();
	bool found_intersection = false;

	for (int i = 0; i < sp.size(); i++)
	{
		found_intersection |= sp[i].intersects(_ray, outRef, outNearest, outMaterial);
	}
	for (int i = 0; i < sphere_point_light.size(); i++)
	{
		found_intersection |= sphere_point_light[i].intersects(_ray, outRef, outNearest, outMaterial);
	}
	for (int i = 0; i < sphere_spot_light.size(); i++)
	{
		found_intersection |= sphere_spot_light[i].intersects(_ray, outRef, outNearest, outMaterial);
	}
	for (int i = 0; i < cube.size(); i++)
	{
		found_intersection |= cube[i].intersects(_ray, outRef, outNearest, outMaterial);
	}

	found_intersection |= floor.intersects(_ray, outRef, outNearest, outMaterial);
}

bool Scene::findIntersection(const ray& _ray, IntersectionQuery& query)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	findIntersectionInternal(_ray, ref, query.nearest, query.material);

	switch (ref.type)
	{
		case ObjRef::IntersectedType::SPHERE:
		{
			Sphere* sphere = static_cast<Sphere*>(ref.object);
			if(query.mover==nullptr)
				query.mover.reset(new SphereMover(sphere));
			break;
		}
		case ObjRef::IntersectedType::CUBE:
		{
			Cube* cube = static_cast<Cube*>(ref.object);
			if (query.mover == nullptr)
				query.mover.reset(new CubeMover(cube));
			break;
		}
		case ObjRef::IntersectedType::SPHERE_POINT_LIGHT:
		{
			Sphere_Point_Light* spl = static_cast<Sphere_Point_Light*>(ref.object);
			if (query.mover == nullptr)
				query.mover.reset(new Sphere_Point_Light_Mover(spl));
			break;
		}
		case ObjRef::IntersectedType::SPHERE_SPOT_LIGHT:
		{
			Sphere_Spot_Light* ssl = static_cast<Sphere_Spot_Light*>(ref.object);
			if (query.mover == nullptr)
				query.mover.reset(new Sphere_Spot_Light_Mover(ssl));
			break;
		}
	}
	return ref.type != ObjRef::IntersectedType::NONE;
}

bool Scene::findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	findIntersectionInternal(_ray, ref, outNearest, outMaterial);
	return ref.type != ObjRef::IntersectedType::NONE;
}

Vec3 Scene::CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, IntersectionQuery& nearest)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	IntersectionQuery back_nearest;
	back_ray.origin = nearest.nearest.point + 0.001f * nearest.nearest.normal;
	for (int i = 0; i < sphere_point_light.size(); i++)
	{
		back_ray.direction = sphere_point_light[i].light.pos - back_ray.origin;
		make_unit_vector(back_ray.direction);
		back_nearest.nearest.reset();
		bool found_intersection = false;
		for (int j = 0; j < sp.size(); j++)
		{
			found_intersection |= sp[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
		}
		for (int j = 0; j < cube.size(); j++)
		{
			found_intersection |= cube[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
		}
		found_intersection |= floor.intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);

		if (found_intersection && (back_nearest.nearest.point - nearest.nearest.point).length() < (sphere_point_light[i].light.pos - nearest.nearest.point).length())
		{
			//if there is another object between the object and the light source
			continue;
		}
		sum += CalculatePointLight(sphere_point_light[i].light, camera, nearest.nearest.point, nearest.nearest.normal, nearest.material);
	}
	return sum;
}

Vec3 Scene::CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, IntersectionQuery& nearest)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	IntersectionQuery back_nearest;
	back_ray.origin = nearest.nearest.point + 0.001f * nearest.nearest.normal;
	for (int i = 0; i < _dir_light.size(); i++)
	{
		back_ray.direction = _dir_light[i].direction;
		make_unit_vector(back_ray.direction);
		back_nearest.nearest.reset();
		bool found_intersection = false;
		for (int j = 0; j < sp.size(); j++)
		{
			found_intersection |= sp[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
		}
		for (int j = 0; j < cube.size(); j++)
		{
			found_intersection |= cube[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
		}
		found_intersection |= floor.intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
		if (found_intersection)
		{
			//if there is another object between the object and the light direction
			continue;
		}
		sum += CalculateDirectionalLight(_dir_light[i], camera, nearest.nearest.point, nearest.nearest.normal, nearest.material);
	}
	return sum;
}

Vec3 Scene::CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, IntersectionQuery& nearest)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	IntersectionQuery back_nearest;
	back_ray.origin = nearest.nearest.point + 0.001f * nearest.nearest.normal;
	for (int i = 0; i < sphere_spot_light.size(); i++)
	{
		Vec3 LightToPixel(nearest.nearest.point - sphere_spot_light[i].light.pos);
		make_unit_vector(LightToPixel);
		make_unit_vector(sphere_spot_light[i].light.direction);
		float spot_factor = dot(LightToPixel, sphere_spot_light[i].light.direction);
		if (spot_factor >= sphere_spot_light[i].light.cutoff)
		{
			back_ray.direction = sphere_spot_light[i].light.pos - back_ray.origin;
			make_unit_vector(back_ray.direction);
			back_nearest.nearest.reset();
			bool found_intersection = false;
			for (int j = 0; j < sp.size(); j++)
			{
				found_intersection |= sp[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
			}
			for (int j = 0; j < cube.size(); j++)
			{
				found_intersection |= cube[j].intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
			}
			found_intersection |= floor.intersects(back_ray, ref, back_nearest.nearest, back_nearest.material);
			if (found_intersection && (back_nearest.nearest.point - nearest.nearest.point).length() < (sphere_spot_light[i].light.pos - nearest.nearest.point).length())
			{
				//if there is another object between the object and the light source
				continue;
			}
			sum += CalculateSpotLight(sphere_spot_light[i].light, camera, nearest.nearest.point, nearest.nearest.normal, nearest.material, spot_factor);
		}
	}
	return sum;
}

void Scene::DrawNearest(IntersectionQuery& nearest, RGBQUAD& pixel)
{
	if (nearest.material.only_emmission)
	{
		pixel.rgbRed = nearest.material.emmission.r();
		pixel.rgbGreen = nearest.material.emmission.g();
		pixel.rgbBlue = nearest.material.emmission.b();
		return;
	}
	Vec3 result_light;
	int color_value;
	Vec3 sum(0, 0, 0);

	pixel.rgbRed = pixel.rgbGreen = pixel.rgbBlue = 0;

	sum += CalculatePointLights(sphere_point_light, nearest);

	sum += CalculateDirectionalLights(dir_light, nearest);

	sum += CalculateSpotLights(sphere_spot_light, nearest);

	result_light = nearest.material.emmission + scene_ambient * nearest.material.albedo + sum;

	if (result_light.r() > 255) result_light.e[0] = 255;
	if (result_light.g() > 255) result_light.e[1] = 255;
	if (result_light.b() > 255) result_light.e[2] = 255;
	pixel.rgbRed = result_light.r();
	pixel.rgbGreen = result_light.g();
	pixel.rgbBlue = result_light.b();
}

void Scene::Redraw(Window& wnd)
{
	ray r(Vec3(0, 0, 0), Vec3(0, 0, 1));
	camera.updateMatrices();
	int pixel_index = 0;
	int size = wnd.image.size();
	IntersectionQuery nearest;
	nearest.reset();
	camera.TopLeft = Vec3(-1, 1, 1);
	camera.BottomLeft = Vec3(-1, -1, 1); camera.BottomRight = Vec3(1, -1, 1);

	camera.TopLeft.mult(camera.m_viewProjInv, 1) / camera.m_viewProjInv[3][3] - camera.position();
	camera.BottomLeft.mult(camera.m_viewProjInv, 1) / camera.m_viewProjInv[3][3] - camera.position();
	camera.BottomRight.mult(camera.m_viewProjInv, 1) / camera.m_viewProjInv[3][3] - camera.position();
	Vec3 BR_M_BL = camera.BottomRight - camera.BottomLeft;
	Vec3 TL_M_BL = camera.TopLeft - camera.BottomLeft;

	for (int y = 0; y < wnd.bitmap_info.bmiHeader.biHeight; y++)
	{
		for (int x = 0; x < wnd.bitmap_info.bmiHeader.biWidth; x++)
		{
			pixel_index = size - wnd.bitmap_info.bmiHeader.biWidth - wnd.bitmap_info.bmiHeader.biWidth * y + x;
			r.origin[0] = (static_cast<float>(x) + 0.5f) / (static_cast<float>(wnd.bitmap_info.bmiHeader.biWidth) - 1.f);
			r.origin[1] = 1.f - (static_cast<float>(y) + 0.5f) / (static_cast<float>(wnd.bitmap_info.bmiHeader.biHeight) - 1.f);
			r.origin[2] = 1;
			r.origin = camera.BottomLeft + BR_M_BL * r.origin[0] + TL_M_BL * r.origin[1];
			r.direction = r.origin - camera.position();

			if (findIntersection(r, nearest))
			{
				DrawNearest(nearest, wnd.image[pixel_index]);
			}
			else
			{
				wnd.image[pixel_index].rgbRed = background_color.r();
				wnd.image[pixel_index].rgbGreen = background_color.g();
				wnd.image[pixel_index].rgbBlue = background_color.b();
			}
		}
	}
}

void Scene::Draw(Window& wnd)
{
	if (need_to_redraw)
	{
		need_to_redraw = false;
		Redraw(wnd);
	}
	StretchDIBits(wnd.device_context, 0, 0, wnd.screen.right, wnd.screen.bottom, 
		0, 0, wnd.bitmap_info.bmiHeader.biWidth, wnd.bitmap_info.bmiHeader.biHeight, &wnd.image[0], &wnd.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}