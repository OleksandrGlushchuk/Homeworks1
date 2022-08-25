#ifndef _PBR_HELPERS_HLSLI_
#define _PBR_HELPERS_HLSLI_
#include "globals.hlsli"
static const float HEMISPHERE_SOLID_ANGLE = 2.f * M_PI;
static const float INSULATOR_F0 = 0.01f;

struct Material
{
    float3 albedo;
    float3 F0;
    float metalness;
    float roughness;
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

inline void clampDirToHorizon(inout float3 dir, inout float NoD, const float3 normal, const float minNoD)
{
    if (NoD < minNoD)
    {
        dir = normalize(dir + (minNoD - NoD) * normal);
        NoD = minNoD;
    }
}

inline float FindSolidAngle(const float DistancePointToLight, const float light_radius, out float lightAngleSin, out float cosHalfAngularDiameter)
{
    lightAngleSin = light_radius / DistancePointToLight;
    cosHalfAngularDiameter = sqrt(1.0f - pow(lightAngleSin, 2));
    return 2.f * M_PI * (1.f - cosHalfAngularDiameter);
}

//D
inline float ggx(float rough2, float NoH, float lightAngleSin = 0, float lightAngleCos = 1)
{
    float denom = NoH * NoH * (rough2 - 1.f) + 1.f;
    denom = M_PI * denom * denom + 0.00001f;
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
    return (1.f - F_LdotN) * (1.f - nearest_mat.metalness) * (solid_angle) * nearest_mat.albedo / M_PI;
}

inline float3 CalculatePointLight(PointLight pointLight, const float3 PointToLight, float3 PointToCameraNormalized, const float3 map_normal, const float3 geometry_normal, const Material nearest_mat)
{
    float3 PointToLightNormalized = normalize(PointToLight);
    
    float DistancePointToLight = length(PointToLight);
    DistancePointToLight = max(DistancePointToLight, pointLight.radius);
    
    float lightAngleSin, cosHalfAngularDiameter;
    float solid_angle = FindSolidAngle(DistancePointToLight, pointLight.radius, lightAngleSin, cosHalfAngularDiameter);
    
    bool intersects = false;
    float3 reflection_dir = reflect(-PointToCameraNormalized, map_normal);
    float3 PointToSpecLight = approximateClosestSphereDir(intersects, reflection_dir, cosHalfAngularDiameter,
		PointToLight, PointToLightNormalized, DistancePointToLight, pointLight.radius);

    
    float NdotL = dot(map_normal, PointToLightNormalized);
    float geometry_fading = dot(geometry_normal, PointToLight);
    float map_fading = dot(map_normal, PointToLight);
    geometry_fading = 1.f - saturate((pointLight.radius - geometry_fading) / (2 * pointLight.radius));
    map_fading = 1.f - saturate((pointLight.radius - map_fading) / (2 * pointLight.radius));
    NdotL = max(NdotL, geometry_fading * lightAngleSin);

    if (NdotL <= 0.f)
        return float3(0.f, 0.f, 0.f);
    
    float NdotV = max(dot(map_normal, PointToCameraNormalized), 0.001f);

    PointToSpecLight = normalize(PointToSpecLight);
    float NdotSpecL = dot(map_normal, PointToSpecLight);
    clampDirToHorizon(PointToSpecLight, NdotSpecL, map_normal, 0.001f);
    
    float3 HalfCameraSpecLight = PointToCameraNormalized + PointToSpecLight;
    HalfCameraSpecLight = normalize(HalfCameraSpecLight);

    float NdotSpecH = max(dot(map_normal, HalfCameraSpecLight), 0.001f);
    float rough2 = nearest_mat.roughness * nearest_mat.roughness;

    float D = ggx(rough2, NdotSpecH);
    float3 F_SpecLdotH = fresnel(dot(PointToSpecLight, HalfCameraSpecLight), nearest_mat.F0);
    float3 F_LdotN = fresnel(NdotL, nearest_mat.F0);
    float G = smith(rough2, NdotV, NdotSpecL);

    return pointLight.radiance * NdotL * map_fading * (LambertBRDF(F_LdotN, nearest_mat, solid_angle) +
    CookTorranceBRDF(F_SpecLdotH, G, D, solid_angle, NdotV, NdotSpecL));
}
#endif