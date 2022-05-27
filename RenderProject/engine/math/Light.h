#pragma once
#include "vec3.h"
#include "camera.h"
#include "../objects/Material.h"

class Light
{
public:
	Vec3 color;
};

class Point_Light : public Light
{
public:
	float light_radius;
	Vec3 pos;
};

class Spot_Light : public Point_Light
{
public:
	Vec3 direction;
	float cutoff;
};

class Directional_Light : public Light
{
public:
	Vec3 direction;
	Vec3 color;
};

inline Vec3 CalculateDirectionalLight(const Directional_Light& light, const Camera& camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 HalfCameraLight((camera.position() - nearest_point)+light.direction);
	make_unit_vector(HalfCameraLight);
	float NdotL = dot(nearest_normal, light.direction);
	float spec = powf(dot(nearest_normal, HalfCameraLight), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo + spec_component);
}

inline Vec3 CalculatePointLight(const Point_Light& light, const Camera &camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToLight(light.pos - nearest_point);
	Vec3 PointToCamera(camera.position() - nearest_point);
	Vec3 HalfCameraLight = PointToCamera + PointToLight;
	make_unit_vector(HalfCameraLight);
	make_unit_vector(PointToLight);
	float DistancePointToLight = length(light.pos - nearest_point);
	float NdotL = dot(nearest_normal, PointToLight);
	float spec = powf(dot(nearest_normal, HalfCameraLight), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo * 1.f / powf(DistancePointToLight / light.light_radius, 2) + spec_component);
}

inline Vec3 CalculateSpotLight(const Spot_Light& light, const Camera& camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat, const float spot_factor)
{
	return CalculatePointLight(light, camera, nearest_point, nearest_normal, nearest_mat)
		* (1.f - (1.f - spot_factor) * 1.f / (1.f - light.cutoff));
}