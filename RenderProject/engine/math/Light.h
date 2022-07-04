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
// sphereCos is cosine of half angular diameter.
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

inline Vec3 CookTorranceBRDF(const Vec3 &F_LdotH, float G, float D, float solid_angle, float NdotV, float NdotSpecL)
{
	return F_LdotH * G * std::min<float>(1.f, solid_angle * D / (4.f * NdotV * NdotSpecL));
}

inline Vec3 LambertBRDF(const Vec3 &F_LdotN, const Material &nearest_mat, float solid_angle)
{
	return (1.f - F_LdotN) * (1.f - nearest_mat.metallness) * (solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI;
}

inline float FindSolidAngle(float DistancePointToLight, float light_radius)
{
	return 2.f * M_PI * (1.f - sqrtf(DistancePointToLight * DistancePointToLight - light_radius * light_radius) / DistancePointToLight);
}

inline float FindCosHalfAngularDiameter(float DistancePointToLight, float light_radius)
{
	return  sqrtf(DistancePointToLight * DistancePointToLight - light_radius * light_radius) / DistancePointToLight; 
}

inline Vec3 CalculateDirectionalLight(const Directional_Light& light, const Vec3& view_pos, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToLight = light.direction;
	Vec3 PointToCamera(view_pos - nearest_point);
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
	return light.radiance * NdotL * ((1.f - F_LdotN) * (1.f - nearest_mat.metallness) * (light.solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI + spec);

}

inline Vec3 CalculatePointLight(const Point_Light& light, const Vec3 &view_pos, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToLight = light.pos - nearest_point;
	Vec3 PointToCamera = view_pos - nearest_point;
	Vec3 HalfCameraLight = PointToCamera + PointToLight;

	float DistancePointToLight = Vec3::length(nearest_point - light.pos);
	float solid_angle;
	DistancePointToLight = std::max<float>(DistancePointToLight, light.light_radius);
	solid_angle = FindSolidAngle(DistancePointToLight, light.light_radius);

	bool intersects = false;
	Vec3 reflection_dir = Vec3::Reflect(-PointToCamera.normalized(), nearest_normal);
	Vec3 PointToSpecLight = approximateClosestSphereDir(intersects, reflection_dir, FindCosHalfAngularDiameter(DistancePointToLight,light.light_radius),
		PointToLight, PointToLight.normalized(), DistancePointToLight, light.light_radius);

	

	PointToCamera.normalize();
	PointToLight.normalize();

	float NdotL = Vec3::dot(nearest_normal, PointToLight);
	if (NdotL <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotV = Vec3::dot(nearest_normal, PointToCamera);
	if (NdotV <= 0.f) return Vec3(0.f, 0.f, 0.f);


	Vec3 HalfCameraSpecLight = PointToCamera + PointToSpecLight;
	HalfCameraLight.normalize();
	HalfCameraSpecLight.normalize();
	PointToSpecLight.normalize();
	
	float NdotSpecL = Vec3::dot(nearest_normal, PointToSpecLight);
	clampDirToHorizon(PointToSpecLight, NdotSpecL, nearest_normal, 0);

	
	float NdotSpecH = Vec3::dot(nearest_normal, HalfCameraSpecLight);
	float NdotH = Vec3::dot(nearest_normal, HalfCameraLight);
	float rough2 = nearest_mat.roughness * nearest_mat.roughness;


	float D = ggx(rough2, NdotSpecH);
	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLight, HalfCameraLight), nearest_mat.F0);
	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotSpecL);

	return light.radiance * NdotL * (LambertBRDF(F_LdotN, nearest_mat, solid_angle) + CookTorranceBRDF(F_LdotH, G, D, solid_angle, NdotV, NdotSpecL));
}

inline float smoothstep(float light_outerCutoff, float light_innerCutoff, float Ldir_dot_LtoPixel)
{
	float x = std::clamp((Ldir_dot_LtoPixel - light_outerCutoff) / (light_innerCutoff - light_outerCutoff), 0.f, 1.f);
	return x * x * (3.f - 2.f * x);
}

inline Vec3 CalculateSpotLight(const Spot_Light& light, const Vec3& view_pos, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat, const float Ldir_dot_LtoPixel)
{
	return CalculatePointLight(light, view_pos, nearest_point, nearest_normal, nearest_mat) * smoothstep(light.outerCutoff, light.innerCutoff, Ldir_dot_LtoPixel);
}


inline Vec3 CalculateSmoothBRDF(const Vec3& input_light, const Vec3 &PointToLightNormalized, const Vec3& view_pos, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	float NdotL = Vec3::dot(nearest_normal, PointToLightNormalized);

	Vec3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
	return input_light * F_LdotN;
}


inline Vec3 CalculateBRDF(const Vec3& input_light, float solid_angle, const Vec3& PointToLightNormalized, const Vec3& view_pos, const Vec3& nearest_point, const Vec3& nearest_normal, const Material& nearest_mat)
{
	Vec3 PointToCamera = view_pos - nearest_point;
	Vec3 HalfCameraLight = PointToCamera.normalize() + PointToLightNormalized;

	float NdotL = Vec3::dot(nearest_normal, PointToLightNormalized);
	if (NdotL <= 0.f) return Vec3(0.f, 0.f, 0.f);
	float NdotV = Vec3::dot(nearest_normal, PointToCamera);
	if (NdotV <= 0.f) return Vec3(0.f, 0.f, 0.f);

	HalfCameraLight.normalize();

	float NdotH = Vec3::dot(nearest_normal, HalfCameraLight);
	float rough2 = nearest_mat.roughness * nearest_mat.roughness;


	float D = ggx(rough2, NdotH);
	Vec3 F_LdotH = fresnel(Vec3::dot(PointToLightNormalized, HalfCameraLight), nearest_mat.F0);
	float G = smith(rough2, NdotV, NdotL);

	return input_light * NdotL * (LambertBRDF(F_LdotH, nearest_mat, solid_angle) + CookTorranceBRDF(F_LdotH, G, D, solid_angle, NdotV, NdotL));
}