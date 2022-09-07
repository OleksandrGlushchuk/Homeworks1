#include "globals.hlsli"
#include "pbr_helpers.hlsli"
#include "environment.hlsli"
Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);

TextureCubeArray<float> g_shadows : register(t4);
static const float shadow_depth_offset = 0.0002f;
static const float shadow_normal_offset = 0.0002f;

cbuffer MeshToModel : register(b1)
{
    float4x4 g_meshToModelMatrix;
}
cbuffer MaterialConstantBuffer : register(b2)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    
    bool g_hasNormalMap;
    bool g_reversedNormalTextureY;
    float2 paddingMCB;
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

void fillSurfaceStructure(out Surface surface, const PS_INPUT input)
{
    surface.albedo = g_colorMap.Sample(g_samplerState, input.tex_coord).xyz;
    surface.metalness = g_hasMetalnessMap ? g_metalnessMap.Sample(g_samplerState, input.tex_coord) : g_metalnessValue;
    surface.roughness = g_hasRoughnessMap ? g_roughnessMap.Sample(g_samplerState, input.tex_coord) : g_roughnessValue;
    surface.F0 = lerp(INSULATOR_F0, surface.albedo, surface.metalness);
    surface.geometry_normal = normalize(input.normal);

    if (g_hasNormalMap)
    {
        surface.map_normal = g_normalMap.Sample(g_samplerState, input.tex_coord);
        surface.map_normal = normalize(surface.map_normal * 2.f - 1.f);
        if (g_reversedNormalTextureY)
            surface.map_normal.y = -surface.map_normal.y;
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        surface.map_normal = normalize(mul(surface.map_normal, TBN));
    }
    else
        surface.map_normal = surface.geometry_normal;
}

void fillViewStructure(out View view, const Surface surface, const PS_INPUT input)
{
    view.PointToCameraNormalized = normalize(g_cameraPos - input.world_pos.xyz);
    view.NdotV = max(dot(surface.map_normal, view.PointToCameraNormalized), 0.001f);
    view.reflectionDir = reflect(-view.PointToCameraNormalized, surface.map_normal);
}

float getDepthInLightSpace(float3 objectWorldPos, uint pointLightIndex)
{
    float dotProduct[6];
    float3 toObject = normalize(objectWorldPos - g_pointLight[pointLightIndex].position);
    dotProduct[0] = dot(float3(1, 0, 0), toObject);
    dotProduct[1] = dot(float3(-1, 0, 0), toObject);
    dotProduct[2] = dot(float3(0, 1, 0), toObject);
    dotProduct[3] = dot(float3(0, -1, 0), toObject);
    dotProduct[4] = dot(float3(0, 0, 1), toObject);
    dotProduct[5] = dot(float3(0, 0, -1), toObject);
    
    uint frustumIndex = 0;
    for (uint i = 1; i < 6; ++i)
    {
        if (dotProduct[i] > dotProduct[frustumIndex])
            frustumIndex = i;
    }
    
    float4 posInLightSpace = mul(float4(objectWorldPos, 1.f), g_viewProjPointLight[pointLightIndex][frustumIndex]);
    return (posInLightSpace.xyz / posInLightSpace.w).z;
}

bool checkForShadow(PS_INPUT input, View view, uint pointLightIndex)
{
    float4 sampleLocation = float4(input.world_pos.xyz - g_pointLight[pointLightIndex].position + shadow_normal_offset*view.PointToCameraNormalized, pointLightIndex);
    float sampleDepth = g_shadows.Sample(g_linearClampSampler, sampleLocation);
    
    float depth_in_light_space = getDepthInLightSpace(input.world_pos.xyz,pointLightIndex);
    return sampleDepth > depth_in_light_space + shadow_depth_offset;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float3 hdrColor = float3(0, 0, 0);
    Surface surface;
    fillSurfaceStructure(surface, input);
    View view;
    fillViewStructure(view, surface, input);
    bool shadow;
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        shadow = checkForShadow(input, view, i);
        hdrColor += shadow ? 0.f : CalculatePointLight(g_pointLight[i], g_pointLight[i].position - input.world_pos.xyz, view, surface);
    }
    hdrColor += addEnvironmentReflection(view, surface);
    return float4(hdrColor, 1.f);
}