#include "Scene.h"
//#include "../math/Matr.h"
//#define  _USE_MATH_DEFINES
//#include <math.h>
//#include <algorithm>
//
//void Scene::findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest, Material& outMaterial)
//{
//	outNearest.reset();
//	bool found_intersection = false;
//
//	for (int i = 0; i < sp.size(); i++)
//	{
//		found_intersection |= sp[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//	for (int i = 0; i < sphere_point_light.size(); i++)
//	{
//		found_intersection |= sphere_point_light[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//	for (int i = 0; i < sphere_spot_light.size(); i++)
//	{
//		found_intersection |= sphere_spot_light[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//	for (int i = 0; i < cube.size(); i++)
//	{
//		found_intersection |= cube[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//
//	found_intersection |= floor.intersects(_ray, outRef, outNearest, outMaterial);
//}
//
//bool Scene::findIntersection(const ray& _ray, IntersectionQuery& query)
//{
//	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
//	findIntersectionInternal(_ray, ref, query.nearest, query.material);
//
//	if (ref.type == ObjRef::IntersectedType::NONE) return false;
//
//	query.nearest.normal.normalize();
//	switch (ref.type)
//	{
//		case ObjRef::IntersectedType::SPHERE:
//		{
//			Sphere* sphere = static_cast<Sphere*>(ref.object);
//			if (query.mover == nullptr)
//			{
//				query.mover.reset(new SphereMover(sphere));
//			}
//			break;
//		}
//		case ObjRef::IntersectedType::CUBE:
//		{
//			Cube* cube = static_cast<Cube*>(ref.object);
//			if (query.mover == nullptr)
//			{
//				query.mover.reset(new CubeMover(cube));
//			}
//			break;
//		}
//		case ObjRef::IntersectedType::SPHERE_POINT_LIGHT:
//		{
//			Sphere_Point_Light* spl = static_cast<Sphere_Point_Light*>(ref.object);
//			if (query.mover == nullptr)
//			{
//				query.mover.reset(new Sphere_Point_Light_Mover(spl));
//			}
//			break;
//		}
//		case ObjRef::IntersectedType::SPHERE_SPOT_LIGHT:
//		{
//			Sphere_Spot_Light* ssl = static_cast<Sphere_Spot_Light*>(ref.object);
//			if (query.mover == nullptr)
//			{
//				query.mover.reset(new Sphere_Spot_Light_Mover(ssl));
//			}
//			break;
//		}
//	}
//	return true;
//}
//
//bool Scene::findIntersection(const ray& _ray, math::Intersection& outNearest, Material& outMaterial, ObjRef& outRef)
//{
//	outRef.type = ObjRef::IntersectedType::NONE;
//	findIntersectionInternal(_ray, outRef, outNearest, outMaterial);
//	return outRef.type != ObjRef::IntersectedType::NONE;
//}
//
//bool Scene::find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest, Material& outMaterial)
//{
//	outNearest.reset();
//	bool found_intersection = false;
//	ObjRef outRef;
//	for (int i = 0; i < sp.size(); i++)
//	{
//		found_intersection |= sp[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//	for (int i = 0; i < cube.size(); i++)
//	{
//		found_intersection |= cube[i].intersects(_ray, outRef, outNearest, outMaterial);
//	}
//	found_intersection |= floor.intersects(_ray, outRef, outNearest, outMaterial);
//
//	return found_intersection;
//}
//
//bool Scene::find_Intersection_Without_Light_Sources(const ray& _ray, math::Intersection& outNearest)
//{
//	bool found_intersection = false;
//	for (int j = 0; j < sp.size(); j++)
//	{
//		found_intersection |= sp[j].intersection(outNearest, _ray);
//	}
//	for (int j = 0; j < cube.size(); j++)
//	{
//		found_intersection |= cube[j].intersection(outNearest, _ray);
//	}
//	found_intersection |= floor.intersection(outNearest, _ray);
//	return found_intersection;
//}
//
//Vec3 Scene::CalculatePointLights(std::vector<Sphere_Point_Light>& _sphere_point_light, const Vec3 &view_pos, const math::Intersection& nearest, const Material& nearest_material)
//{
//	Vec3 sum(0, 0, 0);
//	ray back_ray;
//	math::Intersection back_nearest;
//	back_ray.origin = nearest.point + 0.1f * nearest.normal;
//	for (int i = 0; i < sphere_point_light.size(); ++i)
//	{
//		back_nearest.reset();
//		back_ray.direction = sphere_point_light[i].light.pos - back_ray.origin;
//
//		if (find_Intersection_Without_Light_Sources(back_ray, back_nearest) && 
//			(back_nearest.point - nearest.point).length() < (sphere_point_light[i].light.pos - nearest.point).length())
//		{
//			//if there is another object between the object and the light source
//			continue;
//		}
//		sum += CalculatePointLight(sphere_point_light[i].light, view_pos, nearest.point, nearest.normal, nearest_material);
//	}
//	return sum;
//}
//
//Vec3 Scene::CalculateDirectionalLights(std::vector<Directional_Light>& _dir_light, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material)
//{
//	Vec3 sum(0, 0, 0);
//	ray back_ray;
//	math::Intersection back_nearest;
//	back_ray.origin = nearest.point + 0.1f * nearest.normal;
//	for (int i = 0; i < _dir_light.size(); ++i)
//	{
//		back_ray.direction = _dir_light[i].direction;
//		back_nearest.reset();
//		if (find_Intersection_Without_Light_Sources(back_ray, back_nearest))
//		{
//			//if there is another object between the object and the light direction
//			continue;
//		}
//		sum += CalculateDirectionalLight(_dir_light[i], view_pos, nearest.point, nearest.normal, nearest_material);
//	}
//	return sum;
//}
//
//Vec3 Scene::CalculateSpotLights(std::vector<Sphere_Spot_Light>& _sphere_spot_light, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material)
//{
//	Vec3 sum(0, 0, 0);
//	ray back_ray;
//	math::Intersection back_nearest;
//	back_ray.origin = nearest.point + 0.1f * nearest.normal;
//	for (int i = 0; i < sphere_spot_light.size(); ++i)
//	{
//		Vec3 LightToPixel(nearest.point - sphere_spot_light[i].light.pos);
//		LightToPixel.normalize();
//		sphere_spot_light[i].light.direction.normalize();
//		float Ldir_dot_LtoPixel = Vec3::dot(LightToPixel, sphere_spot_light[i].light.direction);
//		if (Ldir_dot_LtoPixel >= sphere_spot_light[i].light.outerCutoff)
//		{
//			back_ray.direction = sphere_spot_light[i].light.pos - back_ray.origin;
//			back_nearest.reset();
//
//			if (find_Intersection_Without_Light_Sources(back_ray, back_nearest) 
//				&& (back_nearest.point - nearest.point).length() < (sphere_spot_light[i].light.pos - nearest.point).length())
//			{
//				//if there is another object between the object and the light source
//				continue;
//			}
//			sum += CalculateSpotLight(sphere_spot_light[i].light, view_pos, nearest.point, nearest.normal, nearest_material, Ldir_dot_LtoPixel);
//		}
//	}
//	return sum;
//}
//
//ray Scene::RayFromCameraTo(Window& wnd, float x, float y)
//{
//	ray r;
//	r.origin = Vec3(
//		(x + 0.5f) / (wnd.image_width()),
//		1.f - (y + 0.5f) / (wnd.image_height()),
//		1.f
//	);
//	r.origin = camera.BottomLeft + camera.BR_M_BL * r.origin[0] + camera.TL_M_BL * r.origin[1];
//	r.direction = r.origin - camera.position();
//	return r;
//}
//
//Vec3 Scene::CalculateLighting(const ray &ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material, const int & smooth_reflection_depth)
//{
//	Vec3 result_light;
//
//	Vec3 sum = CalculatePointLights(sphere_point_light, view_pos, nearest, nearest_material);
//
//	sum += CalculateDirectionalLights(dir_light, view_pos, nearest, nearest_material);
//
//	sum += CalculateSpotLights(sphere_spot_light, view_pos, nearest, nearest_material);
//
//	Vec3 ambient;
//
//	if (global_illumination)
//	{
//		ambient = CalculateGlobalIllumination(NUMBER_OF_SAMPLES, ray_to_object, view_pos, nearest, nearest_material);
//	}
//	else
//	{
//		ambient = scene_ambient * nearest_material.albedo;
//	}
//	
//	result_light = nearest_material.emmission + ambient + sum;
//
//	if (smooth_reflection)
//	{
//		if (nearest_material.roughness <= MAX_REFLECTIVE_ROUGHNESS && smooth_reflection_depth > 0)
//		{
//			ray r;
//			r.origin = nearest.point + 0.01f * nearest.normal;
//			r.direction = Vec3::Reflect(ray_to_object.direction, nearest.normal);
//
//			math::Intersection reflected_nearest;
//			Material reflected_nearest_material;
//			reflected_nearest.reset();
//
//			Vec3 reflected_light;
//			float smooth_reflection_attenuate = ((MAX_REFLECTIVE_ROUGHNESS - nearest_material.roughness) * 1.f / MAX_REFLECTIVE_ROUGHNESS);
//			if (find_Intersection_Without_Light_Sources(r, reflected_nearest, reflected_nearest_material))
//			{
//				reflected_light = CalculateLighting(r, r.origin, reflected_nearest, reflected_nearest_material, smooth_reflection_depth - 1);
//			}
//			else
//			{
//				reflected_light = background_color;
//			}
//			result_light += CalculateSmoothBRDF(reflected_light, r.direction, view_pos, nearest.point, nearest.normal, nearest_material) * smooth_reflection_attenuate;
//		}
//	}
//	return result_light;
//}
//
//Vec3 Scene::CalculateLightingWithoutGI(const ray& ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material, const int& smooth_reflection_depth)
//{
//	Vec3 result_light;
//
//	Vec3 sum = CalculatePointLights(sphere_point_light, view_pos, nearest, nearest_material);
//
//	sum += CalculateDirectionalLights(dir_light, view_pos, nearest, nearest_material);
//
//	sum += CalculateSpotLights(sphere_spot_light, view_pos, nearest, nearest_material);
//
//	result_light = nearest_material.emmission + scene_ambient * nearest_material.albedo + sum;
//
//	if (smooth_reflection)
//	{
//		if (nearest_material.roughness <= MAX_REFLECTIVE_ROUGHNESS && smooth_reflection_depth > 0)
//		{
//			ray r;
//			r.origin = nearest.point + 0.01f * nearest.normal;
//			r.direction = Vec3::Reflect(ray_to_object.direction, nearest.normal);
//
//			math::Intersection reflected_nearest;
//			Material reflected_nearest_material;
//			reflected_nearest.reset();
//
//			Vec3 reflected_light;
//			float smooth_reflection_attenuate = ((MAX_REFLECTIVE_ROUGHNESS - nearest_material.roughness) * 1.f / MAX_REFLECTIVE_ROUGHNESS);
//			if (find_Intersection_Without_Light_Sources(r, reflected_nearest, reflected_nearest_material))
//			{
//				reflected_light = CalculateLightingWithoutGI(r, r.origin, reflected_nearest, reflected_nearest_material, smooth_reflection_depth - 1);
//			}
//			else
//			{
//				reflected_light = background_color;
//			}
//			result_light += CalculateSmoothBRDF(reflected_light, r.direction, view_pos, nearest.point, nearest.normal, nearest_material) * smooth_reflection_attenuate;
//		}
//	}
//	return result_light;
//}
//
//inline Vec3 fibonacciHemisphereDirection(int current_sample, int number_of_samples)
//{
//	Vec3 res;
//	static const float d_phi = M_PI * (3.f - sqrtf(5.f));
//	float phi = fmodf(d_phi * current_sample, 2.f*M_PI);
//	float d_z = 1.f / number_of_samples;
//	float z = 1.f - d_z / 2.f - current_sample * d_z;
//
//	float theta = acosf(z);
//	float sinf_theta = sinf(theta);
//	res.e[0] = sinf_theta * cosf(phi);
//	res.e[1] = sinf_theta * sinf(phi);
//	res.e[2] = z;
//	return res;
//}
//
//Vec3 Scene::CalculateGlobalIllumination(const int number_of_samples, const ray& ray_to_object, const Vec3& view_pos, const math::Intersection& nearest, const Material& nearest_material)
//{
//	const float d_omega = 2.f * M_PI / number_of_samples;
//
//	Vec3 n_x, n_y;
//	Vec3::GetFrisvadsBasis(nearest.normal, n_x, n_y);
//
//	Vec3 indirect_lighting(0, 0, 0);
//	math::Intersection sampled_nearest;
//	Material sampled_nearest_material;
//	sampled_nearest.reset();
//	ray sampled_ray;
//	sampled_ray.origin = nearest.point + 0.1f * nearest.normal;
//	Vec3 incoming_light;
//
//	for (int j = 0; j < number_of_samples; j++)
//	{
//		sampled_ray.direction = fibonacciHemisphereDirection(j, number_of_samples);
//
//		//Multiplication the sample by a matrix -> translating sample to shaded point local coordinate system
//		sampled_ray.direction = Vec3(
//			sampled_ray.direction.e[0] * n_x.e[0] + sampled_ray.direction.e[1] * n_y.e[0] + sampled_ray.direction.e[2] * nearest.normal.e[0],
//			sampled_ray.direction.e[0] * n_x.e[1] + sampled_ray.direction.e[1] * n_y.e[1] + sampled_ray.direction.e[2] * nearest.normal.e[1],
//			sampled_ray.direction.e[0] * n_x.e[2] + sampled_ray.direction.e[1] * n_y.e[2] + sampled_ray.direction.e[2] * nearest.normal.e[2]
//		);
//
//
//		if (find_Intersection_Without_Light_Sources(sampled_ray, sampled_nearest, sampled_nearest_material))
//		{
//			incoming_light = CalculateLightingWithoutGI(sampled_ray, nearest.point, sampled_nearest, sampled_nearest_material, MAX_DEPTH_FOR_SMOOTH_REFLECTION);
//		}
//		else
//		{
//			incoming_light = scene_ambient;
//		}
//		indirect_lighting += CalculateBRDF(incoming_light, d_omega, sampled_ray.direction, view_pos, nearest.point, nearest.normal, nearest_material);
//	}
//	return (indirect_lighting);
//}
//
//void Scene::LightingPostProcess(Vec3& light)
//{
//	light = lpp::AdjustExposure(light, EV100);
//	light = lpp::ACES(light);
//	light = lpp::GammaCorrection(light);
//}
//
//void Scene::ComputePixelColor(Window& wnd, float x, float y)
//{
//	ray r = RayFromCameraTo(wnd, x, y);
//
//	math::Intersection nearest;
//	Material nearest_material;
//	nearest.reset();
//	Vec3 result_light;
//	ObjRef nearest_object;
//	if (findIntersection(r, nearest, nearest_material, nearest_object))
//	{
//		result_light = (nearest_object.type==ObjRef::IntersectedType::SPHERE_POINT_LIGHT || nearest_object.type==ObjRef::IntersectedType::SPHERE_SPOT_LIGHT) ?
//			nearest_material.emmission : CalculateLighting(r,camera.position(), nearest, nearest_material, MAX_DEPTH_FOR_SMOOTH_REFLECTION);
//	}
//	else
//	{
//		result_light = background_color;
//	}
//
//
//	LightingPostProcess(result_light);
//	int pixel_index = wnd.image.size() - wnd.bitmap_info.bmiHeader.biWidth - wnd.bitmap_info.bmiHeader.biWidth * y + x;
//	RGBQUAD& pixel = wnd.image[pixel_index];
//	result_light *= 255.f;
//	pixel.rgbRed = (BYTE)std::clamp<float>(result_light.e[0], 0, 255);
//	pixel.rgbGreen = (BYTE)std::clamp<float>(result_light.e[1], 0, 255);
//	pixel.rgbBlue = (BYTE)std::clamp<float>(result_light.e[2], 0, 255);
//}
//
//void Scene::Redraw(Window& wnd)
//{
//	ParallelExecutor executor(ParallelExecutor::MAX_THREADS);
//	auto func = [this, &wnd](uint32_t threadIndex, uint32_t taskIndex) {ComputePixelColor(wnd, taskIndex % wnd.image_width(), taskIndex / wnd.image_width()); };
//	executor.execute(func, wnd.image_width() * wnd.image_height(), 50);
//}
//
//void Scene::Draw(Window& wnd)
//{
//	if (need_to_redraw)
//	{
//		Redraw(wnd);
//		need_to_redraw = false;
//	}
//	StretchDIBits(wnd.device_context, 0, 0, wnd.screen.right, wnd.screen.bottom, 
//		0, 0, wnd.bitmap_info.bmiHeader.biWidth, wnd.bitmap_info.bmiHeader.biHeight, &wnd.image[0], &wnd.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
//}


void engine::windows::SceneD3D::Draw()
{
	Redraw();
}

void engine::windows::SceneD3D::Redraw()
{
	for (auto& c : cubes)
		c.Draw();

	sky.Draw();
}

void engine::windows::SceneD3D::findIntersectionInternal(const ray& _ray, ObjRef& outRef, math::Intersection& outNearest)
{
	outNearest.reset();

	for (int i = 0; i < cubes.size(); i++)
	{
		cubes[i].intersects(_ray, outRef, outNearest);
	}
}

bool engine::windows::SceneD3D::findIntersection(const ray& _ray, IntersectionQuery& query)
{
	ObjRef ref = { nullptr, ObjRef::IntersectedType::NONE };
	findIntersectionInternal(_ray, ref, query.nearest);

	if (ref.type == ObjRef::IntersectedType::NONE) return false;

	query.nearest.normal.normalize();
	switch (ref.type)
	{
		case ObjRef::IntersectedType::CUBE:
		{
			Cube* cube = static_cast<Cube*>(ref.object);
			if (query.mover == nullptr)
			{
				query.mover.reset(new CubeMover(cube));
				query.rotator.reset(new CubeRotator(cube));
			}
			break;
		}
	}
	return true;
}
