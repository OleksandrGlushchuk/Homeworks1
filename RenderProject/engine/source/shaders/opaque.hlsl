#include "globals.hlsli"
static const float INSULATOR_F0 = 0.01f;

Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;

    float4 transform_x : TRANSFORM_X;
    float4 transform_y : TRANSFORM_Y;
    float4 transform_z : TRANSFORM_Z;
    float4 transform_w : TRANSFORM_W;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 world_pos : WORLD_POS;
};

PS_INPUT vs_main(VS_INPUT input)
{
    PS_INPUT output;
    float4x4 TransformMatrix = float4x4(input.transform_x, input.transform_y, input.transform_z, input.transform_w);
    
    float4x4 WorldMatrix = mul(g_meshToModelMatrix, TransformMatrix);
    float4 pos = mul(float4(input.position, 1.0f), WorldMatrix);
    output.world_pos = pos;
    pos = mul(pos, g_viewProj);
    
    output.position = pos;
    output.tex_coord = input.tex_coord;
    
    output.normal = normalize(mul((float4(input.normal, 0)), WorldMatrix)).xyz;
    output.tangent = normalize(mul(float4(input.tangent, 0), WorldMatrix)).xyz;
    output.bitangent = normalize(mul(float4(input.bitangent, 0), WorldMatrix)).xyz;

    return output;
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

cbuffer MaterialConstantBuffer : register(b2)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    bool g_hasNormalMap;
    float3 padding;
}

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

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float3 hdrColor = g_colorMap.Sample(g_samplerState, input.tex_coord).xyz;
    Material material;
    material.albedo = hdrColor;
    
    if(g_hasMetalnessMap)
        material.metalness = g_metalnessMap.Sample(g_samplerState, input.tex_coord);
    else
        material.metalness = g_metalnessValue;
    
    if(g_hasRoughnessMap)
        material.roughness = g_roughnessMap.Sample(g_samplerState, input.tex_coord);
    else
        material.roughness = g_roughnessValue;
    
    material.F0 = lerp(INSULATOR_F0, material.albedo, material.metalness);
    
    float3 normal;
    if(g_hasNormalMap)
    {
        normal = g_normalMap.Sample(g_samplerState, input.tex_coord);
        normal = normalize(normal * 2.f - 1.f);
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        normal = mul(normal, TBN);
    }
    else
    {
        normal = input.normal;
    }
    hdrColor = float3(0, 0, 0);
    PointLight pointLight;
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        pointLight.position = g_pointLightBuffer[i].position;
        pointLight.radiance = g_pointLightBuffer[i].radiance;
        pointLight.radius = g_pointLightBuffer[i].radius;
        hdrColor += CalculatePointLight(pointLight, g_cameraPos, input.world_pos.xyz, normal, material);
    }
    return float4(hdrColor, 1.f);
}