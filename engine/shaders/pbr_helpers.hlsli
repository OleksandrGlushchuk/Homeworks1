#ifndef _PBR_HELPERS_HLSLI_
#define _PBR_HELPERS_HLSLI_
#include "globals.hlsli"
static const float HEMISPHERE_SOLID_ANGLE = 2.f * M_PI;
static const float INSULATOR_F0 = 0.01f;
static const float DIRECTIONAL_LIGHT_SOLID_ANGLE = M_PI;

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

inline float3 CookTorranceBRDF(const float3 F_LdotH, float G, float D, float solid_angle, float NdotV)
{
    return F_LdotH * G * min(1.f, solid_angle * D / (4.f * NdotV));
}

inline float3 LambertBRDF(const float3 F_LdotN, const Surface surface, float solid_angle)
{
    return (1.f - F_LdotN) * (1.f - surface.metalness) * (solid_angle) * surface.albedo / M_PI;
}

inline float3 CalculatePointLight(PointLight pointLight, const float3 PointToLight, const View view, const Surface surface)
{
    float GNdotL = dot(surface.geometry_normal, PointToLight);
    if (GNdotL <= -pointLight.radius)
        return 0;
    
    float DistancePointToLight = length(PointToLight);
    DistancePointToLight = max(DistancePointToLight, pointLight.radius);
    
    float lightAngleSin, cosHalfAngularDiameter;
    float solid_angle = FindSolidAngle(DistancePointToLight, pointLight.radius, lightAngleSin, cosHalfAngularDiameter);
    
    float geometry_fading = 1.f - saturate((pointLight.radius - GNdotL) / (2 * pointLight.radius));
    float map_fading = dot(surface.map_normal, PointToLight);
    map_fading = 1.f - saturate((pointLight.radius - map_fading) / (2 * pointLight.radius));
    
    float3 PointToLightNormalized = normalize(PointToLight);
    float NdotL = dot(surface.map_normal, PointToLightNormalized);
    NdotL = max(NdotL, map_fading * lightAngleSin);
    
    bool intersects = false;
    
    float3 PointToSpecLight = approximateClosestSphereDir(intersects, view.reflectionDir, cosHalfAngularDiameter,
	PointToLight, PointToLightNormalized, DistancePointToLight, pointLight.radius);

    PointToSpecLight = normalize(PointToSpecLight);
    float NdotSpecL = dot(surface.map_normal, PointToSpecLight);
    clampDirToHorizon(PointToSpecLight, NdotSpecL, surface.map_normal, 0.001f);
    
    float3 HalfCameraSpecLight = view.PointToCameraNormalized + PointToSpecLight;
    HalfCameraSpecLight = normalize(HalfCameraSpecLight);

    float NdotSpecH = max(dot(surface.map_normal, HalfCameraSpecLight), 0.001f);
    float rough2 = surface.roughness * surface.roughness;

    float D = ggx(rough2, NdotSpecH);
    float3 F_SpecLdotH = fresnel(dot(PointToSpecLight, HalfCameraSpecLight), surface.F0);
    float3 F_LdotN = fresnel(NdotL, surface.F0);
    float G = smith(rough2, view.NdotV, NdotSpecL);

    return pointLight.radiance * map_fading * geometry_fading * (NdotL * LambertBRDF(F_LdotN, surface, solid_angle) +
    CookTorranceBRDF(F_SpecLdotH, G, D, solid_angle, view.NdotV));
}

inline float3 CalculateDirectionalLight(DirectionalLight light, const View view, const Surface surface)
{
    float NdotL = dot(surface.map_normal, light.direction);
    if (NdotL <= 0.f)
        return 0;

    float3 HalfCameraLight = view.PointToCameraNormalized + light.direction;
    HalfCameraLight = normalize(HalfCameraLight);

    float NdotH = dot(surface.map_normal, HalfCameraLight);
    float rough2 = surface.roughness * surface.roughness;
    float D = ggx(rough2, NdotH);

    float3 F_LdotH = fresnel(dot(light.direction, HalfCameraLight), surface.F0);
    float3 F_LdotN = fresnel(NdotL, surface.F0);
    float G = smith(rough2, view.NdotV, NdotL);

    return light.radiance * (NdotL * LambertBRDF(F_LdotN, surface, DIRECTIONAL_LIGHT_SOLID_ANGLE) +
    CookTorranceBRDF(F_LdotH, G, D, DIRECTIONAL_LIGHT_SOLID_ANGLE, view.NdotV));
}
#endif