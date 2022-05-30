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

	if (ref.type == ObjRef::IntersectedType::NONE) return false;

	query.nearest.normal.normalize();
	switch (ref.type)
	{
		case ObjRef::IntersectedType::SPHERE:
		{
			Sphere* sphere = static_cast<Sphere*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new SphereMover(sphere));
			}
			break;
		}
		case ObjRef::IntersectedType::CUBE:
		{
			Cube* cube = static_cast<Cube*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new CubeMover(cube));
			}
			break;
		}
		case ObjRef::IntersectedType::SPHERE_POINT_LIGHT:
		{
			Sphere_Point_Light* spl = static_cast<Sphere_Point_Light*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new Sphere_Point_Light_Mover(spl));
			}
			break;
		}
		case ObjRef::IntersectedType::SPHERE_SPOT_LIGHT:
		{
			Sphere_Spot_Light* ssl = static_cast<Sphere_Spot_Light*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new Sphere_Spot_Light_Mover(ssl));
			}
			break;
		}
	}
	return true;
}

bool Scene::findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	findIntersectionInternal(_ray, ref, outNearest, outMaterial);
	return ref.type != ObjRef::IntersectedType::NONE;
}

Vec3 Scene::CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, math::Intersection& nearest, Material& nearest_material)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.001f * nearest.normal;
	for (int i = 0; i < sphere_point_light.size(); i++)
	{
		back_ray.direction = sphere_point_light[i].light.pos - back_ray.origin;
		back_ray.direction.normalize();
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

		if (found_intersection && (back_nearest.point - nearest.point).length() < (sphere_point_light[i].light.pos - nearest.point).length())
		{
			//if there is another object between the object and the light source
			continue;
		}
		sum += CalculatePointLight(sphere_point_light[i].light, camera, nearest.point, nearest.normal, nearest_material);
	}
	return sum;
}

Vec3 Scene::CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, math::Intersection& nearest, Material& nearest_material)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.001f * nearest.normal;
	for (int i = 0; i < _dir_light.size(); i++)
	{
		back_ray.direction = _dir_light[i].direction;
		back_ray.direction.normalize();
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
		if (found_intersection)
		{
			//if there is another object between the object and the light direction
			continue;
		}
		sum += CalculateDirectionalLight(_dir_light[i], camera, nearest.point, nearest.normal, nearest_material);
	}
	return sum;
}

Vec3 Scene::CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, math::Intersection& nearest, Material& nearest_material)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.001f * nearest.normal;
	for (int i = 0; i < sphere_spot_light.size(); i++)
	{
		Vec3 LightToPixel(nearest.point - sphere_spot_light[i].light.pos);
		LightToPixel.normalize();
		sphere_spot_light[i].light.direction.normalize();
		float Ldir_dot_LtoPixel = Vec3::dot(LightToPixel, sphere_spot_light[i].light.direction);
		if (Ldir_dot_LtoPixel >= sphere_spot_light[i].light.outerCutoff)
		{
			back_ray.direction = sphere_spot_light[i].light.pos - back_ray.origin;
			back_ray.direction.normalize();
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
			if (found_intersection && (back_nearest.point - nearest.point).length() < (sphere_spot_light[i].light.pos - nearest.point).length())
			{
				//if there is another object between the object and the light source
				continue;
			}
			sum += CalculateSpotLight(sphere_spot_light[i].light, camera, nearest.point, nearest.normal, nearest_material, Ldir_dot_LtoPixel);
		}
	}
	return sum;
}

void Scene::DrawNearest(math::Intersection& nearest, Material& nearest_material, RGBQUAD& pixel)
{
	if (nearest_material.only_emmission)
	{
		pixel.rgbRed = nearest_material.emmission.r();
		pixel.rgbGreen = nearest_material.emmission.g();
		pixel.rgbBlue = nearest_material.emmission.b();
		return;
	}
	Vec3 result_light;
	int color_value;
	Vec3 sum(0, 0, 0);

	pixel.rgbRed = pixel.rgbGreen = pixel.rgbBlue = 0;

	sum += CalculatePointLights(sphere_point_light, nearest, nearest_material);

	sum += CalculateDirectionalLights(dir_light, nearest, nearest_material);

	sum += CalculateSpotLights(sphere_spot_light, nearest, nearest_material);

	result_light = nearest_material.emmission + scene_ambient * nearest_material.albedo + sum;

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
	math::Intersection nearest;
	Material nearest_material;

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
			r.origin[0] = (x + 0.5f) / (wnd.bitmap_info.bmiHeader.biWidth - 1.f);
			r.origin[1] = 1.f - (y + 0.5f) / (wnd.bitmap_info.bmiHeader.biHeight - 1.f);
			r.origin[2] = 1;
			r.origin = camera.BottomLeft + BR_M_BL * r.origin[0] + TL_M_BL * r.origin[1];
			r.direction = r.origin - camera.position();

			if (findIntersection(r, nearest, nearest_material))
			{
				DrawNearest(nearest, nearest_material, wnd.image[pixel_index]);
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