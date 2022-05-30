#pragma once
#include "vec3.h"
#include "camera.h"
#include "../objects/Material.h"
#include <algorithm>

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
	float outerCutoff;
	float innerCutoff;
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
	HalfCameraLight.normalize();
	float NdotL = Vec3::dot(nearest_normal, light.direction);
	float spec = powf(Vec3::dot(nearest_normal, HalfCameraLight), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo + spec_component);
}

inline Vec3 CalculatePointLight(const Point_Light& light, const Camera &camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToLight(light.pos - nearest_point);
	Vec3 PointToCamera(camera.position() - nearest_point);
	Vec3 HalfCameraLight = PointToCamera + PointToLight;
	HalfCameraLight.normalize();
	PointToLight.normalize();
	float DistancePointToLight = Vec3::length(light.pos - nearest_point);
	float NdotL = Vec3::dot(nearest_normal, PointToLight);
	float spec = powf(Vec3::dot(nearest_normal, HalfCameraLight), nearest_mat.glossiness) * nearest_mat.specular;
	Vec3 spec_component(spec, spec, spec);
	return light.color * std::max<float>(NdotL, 0) * (NdotL * nearest_mat.albedo * 1.f / powf(DistancePointToLight / light.light_radius, 2) + spec_component);
}

inline float smoothstep(float edge0, float edge1, float x)
{
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return x * x * (3 - 2 * x);
}

inline Vec3 CalculateSpotLight(const Spot_Light& light, const Camera& camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat, const float Ldir_dot_LtoPixel)
{
	return CalculatePointLight(light, camera, nearest_point, nearest_normal, nearest_mat) * smoothstep(light.outerCutoff, light.innerCutoff, Ldir_dot_LtoPixel);
}