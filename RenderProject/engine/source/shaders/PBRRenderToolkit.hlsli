#ifndef _PBRRENDERTOOLKIT_HLSLI_
#define _PBRRENDERTOOLKIT_HLSLI_
#include "globals.hlsli"
static const float HALF_SPHERE_SOLID_ANGLE = 2.f * M_PI;
static const float INSULATOR_F0 = 0.01f;

cbuffer MaterialConstantBuffer : register(b2)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    bool g_hasNormalMap;
    float3 padding;
}

struct Material
{
    float3 albedo;
    float3 F0;
    float metalness;
    float roughness;
};

struct PointLight
{
    float3 radiance;
    float3 position;
    float radius;
};

inline float3 approximateClosestSphereDir(inout bool intersects, const float3 reflectionDir, float sphereCos,
	const float3 sphereRelPos, const float3 sphereDir, float sphereDist, float sphereRadius)
{
    float RoS = dot(reflectionDir, sphereDir);

    intersects = RoS >= sphereCos;
    if (intersects)
        return reflectionDir;
    if (RoS < 0.0)
        return sphereDir;

    float3 closestPointDir = normalize(reflectionDir * sphereDist * RoS - sphereRelPos);
    return normalize(sphereRelPos + sphereRadius * closestPointDir);
}

inline void clampDirToHorizon(float3 dir, inout float NoD, const float3 normal, float minNoD)
{
    if (NoD < minNoD)
    {
        dir = normalize(dir + (minNoD - NoD) * normal);
        NoD = minNoD;
    }
}

inline float FindSolidAngle(float DistancePointToLight, float light_radius)
{
    return 2.f * M_PI * (1.f - sqrt(DistancePointToLight * DistancePointToLight - light_radius * light_radius) / DistancePointToLight);
}

inline float FindCosHalfAngularDiameter(float DistancePointToLight, float light_radius)
{
    return sqrt(DistancePointToLight * DistancePointToLight - light_radius * light_radius) / DistancePointToLight;
}

//D
inline float ggx(float rough2, float NoH, float lightAngleSin = 0, float lightAngleCos = 1)
{
    float denom = NoH * NoH * (rough2 - 1.f) + 1.f;
    denom = M_PI * denom * denom;
    return rough2 / denom;
}

//F
inline float3 fresnel(float NdotL, const float3 F0)
{
    return F0 + (1.f - F0) * pow(1.f - NdotL, 5);
}

//G
inline float smith(float rough2, float NoV, float NoL)
{
    NoV *= NoV;
    NoL *= NoL;
    return 2.f / (sqrt(1 + rough2 * (1 - NoV) / NoV) + sqrt(1 + rough2 * (1 - NoL) / NoL));
}

inline float3 CookTorranceBRDF(const float3 F_LdotH, float G, float D, float solid_angle, float NdotV, float NdotSpecL)
{
    return F_LdotH * G * min(1.f, solid_angle * D / (4.f * NdotV * NdotSpecL));
}

inline float3 LambertBRDF(const float3 F_LdotN, const Material nearest_mat, float solid_angle)
{
    return (1.f - F_LdotN) * (1.f - nearest_mat.metalness) * (solid_angle / HALF_SPHERE_SOLID_ANGLE) * nearest_mat.albedo / M_PI;
}

inline float3 CalculatePointLight(PointLight pointLight, const float3 view_pos, const float3 nearest_point, const float3 nearest_normal, const Material nearest_mat)
{
    float3 PointToLight = pointLight.position - nearest_point;
    float3 PointToCamera = view_pos - nearest_point;
    float3 HalfCameraLight = PointToCamera + PointToLight;

    float DistancePointToLight = length(nearest_point - pointLight.position);
    
    DistancePointToLight = max(DistancePointToLight, pointLight.radius);
    float solid_angle = FindSolidAngle(DistancePointToLight, pointLight.radius);
    
    bool intersects = false;
    float3 reflection_dir = reflect(-normalize(PointToCamera), nearest_normal);
    float3 PointToSpecLight = approximateClosestSphereDir(intersects, reflection_dir, FindCosHalfAngularDiameter(DistancePointToLight, pointLight.radius),
		PointToLight, normalize(PointToLight), DistancePointToLight, pointLight.radius);

    PointToCamera = normalize(PointToCamera);
    PointToLight = normalize(PointToLight);

    float NdotL = dot(nearest_normal, PointToLight);
    if (NdotL <= 0.f)
        return float3(0.f, 0.f, 0.f);
    float NdotV = dot(nearest_normal, PointToCamera);
    if (NdotV <= 0.f)
        return float3(0.f, 0.f, 0.f);


    float3 HalfCameraSpecLight = PointToCamera + PointToSpecLight;
    HalfCameraLight = normalize(HalfCameraLight);
    HalfCameraSpecLight = normalize(HalfCameraSpecLight);
    PointToSpecLight = normalize(PointToSpecLight);
	
    float NdotSpecL = dot(nearest_normal, PointToSpecLight);
    clampDirToHorizon(PointToSpecLight, NdotSpecL, nearest_normal, 0);

	
    float NdotSpecH = dot(nearest_normal, HalfCameraSpecLight);
    float NdotH = dot(nearest_normal, HalfCameraLight);
    float rough2 = nearest_mat.roughness * nearest_mat.roughness;


    float D = ggx(rough2, NdotSpecH);
    float3 F_LdotH = fresnel(dot(PointToLight, HalfCameraLight), nearest_mat.F0);
    float3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
    float G = smith(rough2, NdotV, NdotSpecL);

    return pointLight.radiance * NdotL * (LambertBRDF(F_LdotN, nearest_mat, solid_angle) + CookTorranceBRDF(F_LdotH, G, D, solid_angle, NdotV, NdotSpecL));
}
#endif