#pragma once
#include "vec3.h"
#include "camera.h"
#include "../objects/Material.h"
#include <algorithm>
static const float HALF_SPHERE_SOLID_ANGLE = 2.f * M_PI;

class Light
{
public:
	Vec3 radiance;
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
	Vec3 radiance;
	float solid_angle;
};

// May return direction pointing beneath surface horizon (dot(N, dir) < 0), use clampDirToHorizon to fix it.
// sphereCos is cosine of light sphere solid angle.
// sphereRelPos is position of a sphere relative to surface:
// 'sphereDir == normalize(sphereRelPos)' and 'sphereDir * sphereDist == sphereRelPos'
inline Vec3 approximateClosestSphereDir(bool& intersects, const Vec3& reflectionDir, float sphereCos,
	const Vec3& sphereRelPos, const Vec3& sphereDir, float sphereDist, float sphereRadius)
{
	float RoS = Vec3::dot(reflectionDir, sphereDir);

	intersects = RoS >= sphereCos;
	if (intersects) return reflectionDir;
	if (RoS < 0.0) return sphereDir;

	Vec3 closestPointDir = (reflectionDir * sphereDist * RoS - sphereRelPos).normalize();
	return (sphereRelPos + sphereRadius * closestPointDir).normalized();
}

// Input dir and NoD is N and NoL in a case of lighting computation 
inline void clampDirToHorizon(Vec3& dir, float& NoD, const Vec3& normal, float minNoD)
{
	if (NoD < minNoD)
	{
		dir = (dir + (minNoD - NoD) * normal).normalize();
		NoD = minNoD;
	}
}

//D
inline float ggx(float rough2, float NoH, float lightAngleSin = 0, float lightAngleCos = 1)
{
	float denom = NoH * NoH * (rough2 - 1.f) + 1.f;
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

inline float FindSolidAngle(float DistancePointToLight, float light_radius)
{
	return 2.f * M_PI * (1.f - sqrt(DistancePointToLight * DistancePointToLight - light_radius * light_radius) / DistancePointToLight);
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
	float D = ggx(rough2,NdotH);

	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);
	Vec3 spec = F_LdotH * G * std::min<float>(1.f, light.solid_angle * D / (4.f * NdotV * NdotL));
	return light.radiance * NdotL * ((1.f - F_LdotN) * (1 - nearest_mat.metallness) * (light.solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI + spec);

}

inline Vec3 CalculatePointLight(const Point_Light& light, const Camera &camera, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	/*Vec3 PointToLight(light.pos - nearest_point);
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


	float D = ggx(rough2, NdotH);
	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);

	float DistancePointToLight = Vec3::length(nearest_point - light.pos);
	float solid_angle;
	if (DistancePointToLight >= light.light_radius)
		solid_angle = FindSolidAngle(DistancePointToLight,light.light_radius);
	else
		solid_angle = HALF_SPHERE_SOLID_ANGLE;


	Vec3 spec = F_LdotH * G * std::min<float>(1.f, solid_angle * D / (4.f * NdotV * NdotL));

	return light.radiance * NdotL * ((1.f - F_LdotN) * (1 - nearest_mat.metallness) * (solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI + spec);*/


	Vec3 PointToLight = light.pos - nearest_point;
	float DistancePointToLight = Vec3::length(nearest_point - light.pos);
	float solid_angle;
	if (DistancePointToLight >= light.light_radius)
		solid_angle = FindSolidAngle(DistancePointToLight, light.light_radius);
	else
		solid_angle = HALF_SPHERE_SOLID_ANGLE;

	bool intersects = false;
	ray r;
	r.origin = nearest_point + 0.01f * nearest_normal;
	r.direction = Vec3::Reflect(-PointToLight.normalized(), nearest_normal);
	Vec3 PointToSpecLight = approximateClosestSphereDir(intersects, r.direction, cosf(solid_angle), PointToLight, PointToLight.normalized(), DistancePointToLight, light.light_radius);
	
	Vec3 PointToCamera = camera.position() - nearest_point;
	Vec3 HalfCameraLight = PointToCamera + PointToLight;
	Vec3 HalfCameraSpecLight = PointToCamera + PointToSpecLight;
	
	PointToCamera.normalize();
	PointToLight.normalize();

	float NdotL = Vec3::dot(nearest_normal, PointToLight);
	if (NdotL <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotV = Vec3::dot(nearest_normal, PointToCamera);
	if (NdotV <= 0.f) return Vec3(0.f, 0.f, 0.f);

	HalfCameraLight.normalize();
	HalfCameraSpecLight.normalize();
	
	
	float NdotSpecL = Vec3::dot(nearest_normal, PointToSpecLight);
	clampDirToHorizon(PointToSpecLight, NdotSpecL, nearest_normal, 0);

	
	float NdotSpecH = Vec3::dot(nearest_normal, HalfCameraSpecLight);
	float NdotH = Vec3::dot(nearest_normal, HalfCameraSpecLight);
	float rough2 = nearest_mat.roughness * nearest_mat.roughness;


	float D = ggx(rough2, NdotSpecH);
	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);
	

	Vec3 spec = F_LdotH * G * std::min<float>(1.f, solid_angle * D / (4.f * NdotV * NdotSpecL));

	return light.radiance * NdotL * ((1.f - F_LdotN) * (1 - nearest_mat.metallness) * (solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI + spec);
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