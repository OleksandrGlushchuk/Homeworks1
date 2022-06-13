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
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.1f * nearest.normal;
	for (int i = 0; i < sphere_point_light.size(); ++i)
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
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.1f * nearest.normal;
	for (int i = 0; i < _dir_light.size(); ++i)
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
	Vec3 sum(0, 0, 0);
	ray back_ray;
	math::Intersection back_nearest;
	back_ray.origin = nearest.point + 0.1f * nearest.normal;
	for (int i = 0; i < sphere_spot_light.size(); ++i)
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

ray Scene::RayFromCameraTo(Window& wnd, float x, float y)
{
	ray r;
	r.origin = Vec3(
		(x + 0.5f) / (wnd.image_width() - 1.f),
		1.f - (y + 0.5f) / (wnd.image_height() - 1.f),
		1.f
	);
	r.origin = camera.BottomLeft + camera.BR_M_BL * r.origin[0] + camera.TL_M_BL * r.origin[1];
	r.direction = r.origin - camera.position();
	return r;
}

Vec3 Scene::CalculateLighting(const ray &ray_to_object, math::Intersection& nearest, Material& nearest_material, const int &depth)
{
	Vec3 result_light;
	if (nearest_material.only_emmission)
	{
		result_light = nearest_material.emmission;
		return result_light;
	}
	Vec3 sum(0, 0, 0);

	sum += CalculatePointLights(sphere_point_light, nearest, nearest_material);

	sum += CalculateDirectionalLights(dir_light, nearest, nearest_material);

	sum += CalculateSpotLights(sphere_spot_light, nearest, nearest_material);

	result_light = nearest_material.emmission + scene_ambient * nearest_material.albedo + sum;
	
	if (smooth_reflection)
	{
		if (nearest_material.roughness <= MAX_REFLECTIVE_ROUGHNESS && depth > 0)
		{
			ray r;
			r.origin = nearest.point + 0.01f * nearest.normal;
			r.direction = Vec3::Reflect(ray_to_object.direction, nearest.normal);

			math::Intersection reflected_nearest;
			Material reflected_nearest_material;
			reflected_nearest.reset();
			if (find_Intersection_Without_Light_Sources(r, reflected_nearest, reflected_nearest_material))
			{
				result_light += CalculateLighting(r, reflected_nearest, reflected_nearest_material, depth - 1) *
					Vec3::lerp(Vec3(0, 0, 0), result_light, ((MAX_REFLECTIVE_ROUGHNESS - nearest_material.roughness) * 1.f/ MAX_REFLECTIVE_ROUGHNESS));
			}
			else
			{
				result_light += background_color;
			}
		}
	}
	return result_light;
}

bool Scene::find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest, Material& outMaterial)
{
	outNearest.reset();
	bool found_intersection = false;
	ObjRef outRef;
	for (int i = 0; i < sp.size(); i++)
	{
		found_intersection |= sp[i].intersects(_ray, outRef, outNearest, outMaterial);
	}
	for (int i = 0; i < cube.size(); i++)
	{
		found_intersection |= cube[i].intersects(_ray, outRef, outNearest, outMaterial);
	}
	found_intersection |= floor.intersects(_ray, outRef, outNearest, outMaterial);

	return found_intersection;
}

void Scene::LightningPostProcess(Vec3& light)
{
	light = lpp::AdjustExposure(light, EV100);
	light = lpp::ACES(light);
	light = lpp::GammaCorrection(light);
}

void Scene::ComputePixelColor(Window& wnd, float x, float y)
{
	ray r = RayFromCameraTo(wnd, x, y);

	math::Intersection nearest;
	Material nearest_material;
	nearest.reset();
	Vec3 result_light;
	if (findIntersection(r, nearest, nearest_material))
	{
		result_light = CalculateLighting(r, nearest, nearest_material,50);
	}
	else
	{
		result_light = background_color;
	}
	LightningPostProcess(result_light);

	int pixel_index = wnd.image.size() - wnd.bitmap_info.bmiHeader.biWidth - wnd.bitmap_info.bmiHeader.biWidth * y + x;
	RGBQUAD& pixel = wnd.image[pixel_index];
	result_light *= 255.f;
	pixel.rgbRed = (BYTE)std::clamp<float>(result_light.e[0], 0, 255);
	pixel.rgbGreen = (BYTE)std::clamp<float>(result_light.e[1], 0, 255);
	pixel.rgbBlue = (BYTE)std::clamp<float>(result_light.e[2], 0, 255);
}

void Scene::Redraw(Window& wnd)
{
	camera.updateCorners();
	ParallelExecutor executor(ParallelExecutor::MAX_THREADS);
	auto func = [this,&wnd](uint32_t threadIndex, uint32_t taskIndex){ComputePixelColor(wnd, taskIndex % wnd.image_width(), taskIndex % wnd.image_height()); };
	executor.execute(func, wnd.image_width() * wnd.image_height(), 200);
}

void Scene::Draw(Window& wnd)
{
	if (need_to_redraw)
	{
		Redraw(wnd);
		need_to_redraw = false;
	}
	StretchDIBits(wnd.device_context, 0, 0, wnd.screen.right, wnd.screen.bottom, 
		0, 0, wnd.bitmap_info.bmiHeader.biWidth, wnd.bitmap_info.bmiHeader.biHeight, &wnd.image[0], &wnd.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
}