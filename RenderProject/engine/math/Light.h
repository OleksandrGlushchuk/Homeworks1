#pragma once
#include "vec3.h"
#include "camera.h"
#include "../objects/Material.h"
#include <algorithm>
static const float HALF_SPHERE_SOLID_ANGLE = 2.f * M_PI;

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

inline float GGX_Distribution(float cosThetaNH, float alpha) {
	float alpha2 = alpha * alpha;
	float NH_sqr = (cosThetaNH * cosThetaNH);
	float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
	return alpha2 / (M_PI * den * den);
}
//D
inline float ggx(float rough2, float NoH, float lightAngleSin = 0, float lightAngleCos = 1)
{
	float denom = NoH * NoH * (rough2 - 1.0) + 1.0;
	denom = M_PI * denom * denom;
	return rough2 / denom;
}

//F
inline Vec3 fresnel(float NdotL, const Vec3& F0)
{
	return F0 + (1.f - F0) * powf(1.f - NdotL, 5);
}

//G
inline float smith(float rough2, float NoV, float NoL)
{
	NoV *= NoV;
	NoL *= NoL;
	return 2.f / (sqrtf(1 + rough2 * (1 - NoV) / NoV) + sqrtf(1 + rough2 * (1 - NoL) / NoL));
}


inline Vec3 CalculateDirectionalLight(const Directional_Light& light, const Camera& camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{

	Vec3 PointToLight = light.direction;
	Vec3 PointToCamera(camera.position() - nearest_point);
	Vec3 HalfCameraLight = PointToCamera + PointToLight;
	HalfCameraLight.normalize();
	PointToCamera.normalize();
	PointToLight.normalize();

	float NdotL = Vec3::dot(nearest_normal, PointToLight);
	if (NdotL <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotV = Vec3::dot(nearest_normal, PointToCamera);
	if (NdotV <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotH = Vec3::dot(nearest_normal, HalfCameraLight);
	float rough2 = nearest_mat.roughness * nearest_mat.roughness;
	float D = GGX_Distribution(NdotH, rough2);

	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);

	Vec3 spec = 0.25f * D * F_LdotH * G * nearest_mat.metallness / (NdotV * NdotL);
	return light.color * NdotL * ((1.f - F_LdotN) * (1 - nearest_mat.metallness) * nearest_mat.albedo / M_PI + spec);
}

inline Vec3 CalculatePointLight(const Point_Light& light, const Camera &camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToLight(light.pos - nearest_point);
	Vec3 PointToCamera(camera.position() - nearest_point);
	Vec3 HalfCameraLight = PointToCamera + PointToLight;
	HalfCameraLight.normalize();
	PointToCamera.normalize();
	PointToLight.normalize();

	float DistancePointToLight = Vec3::length(nearest_point - light.pos);
	float NdotL = Vec3::dot(nearest_normal, PointToLight);
	if (NdotL <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotV = Vec3::dot(nearest_normal, PointToCamera);
	if (NdotV <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotH = Vec3::dot(nearest_normal, HalfCameraLight);
	float rough2 = nearest_mat.roughness * nearest_mat.roughness;
	float D = GGX_Distribution(NdotH, rough2);

	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);
	Vec3 spec = 0.25f * D * F_LdotH * G * nearest_mat.metallness / (NdotV * NdotL);
	float new_radius = light.light_radius / DistancePointToLight;
	float solid_angle = M_PI * new_radius * new_radius;
	return light.color * NdotL * ((1.f - F_LdotN) * (1 - nearest_mat.metallness) * (solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI + spec);

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