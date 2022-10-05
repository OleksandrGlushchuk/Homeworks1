#include "globals.hlsli"

static const uint TEXTURES_IN_ONE_BUSH = 6;
static const uint MAX_VERTEX_COUNT = TEXTURES_IN_ONE_BUSH * 3;

Texture2D g_colorMap : register(t0);
Texture2D<float3> g_normalMap : register(t1);
Texture2D<float> g_roughnessMap : register(t2);
Texture2D<float> g_metalnessMap : register(t3);
Texture2D<float> g_AOMap : register(t4);
Texture2D<float> g_opacityMap : register(t5);
Texture2D<float3> g_translucency : register(t6);


struct VS_INPUT
{
    float3 grass_pos : GRASS_POS;
};

struct GS_INPUT
{
    float3 grass_pos : GRASS_POS;
    float3 vertex_pos : VERTEX_POS;
    float2 tex_coord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    float3 world_pos : WORLD_POS;
    float2 tex_coord : TEXCOORD0;
    float3 normal : PNORMAL;
    float3 tangent : PTANGENT;
    float3 bitangent : PBITANGENT;
};

GS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    GS_INPUT output;
    float3 pos;
    pos.z = 0;
    if (vertexID == 0 || vertexID == 5)
    {
        pos.xy = float2(-0.5f, -0.5f);
        output.tex_coord = float2(0, 1);
    }
    else if (vertexID == 1)
    {
        pos.xy = float2(-0.5f, 0.5f);
        output.tex_coord = float2(0, 0);
    }
    else if (vertexID == 2 || vertexID == 3)
    {
        pos.xy = float2(0.5f, 0.5f);
        output.tex_coord = float2(1, 0);
    }
    else if(vertexID == 4)
    {
        pos.xy = float2(0.5f, -0.5f);
        output.tex_coord = float2(1, 1);
    }

    output.vertex_pos = pos;
    output.grass_pos = input.grass_pos;
    output.normal = float3(0, 0, -1.f);
    output.tangent = float3(1.f, 0, 0);
    output.bitangent = float3(0, -1.f, 0);
    return output;
}

[maxvertexcount(MAX_VERTEX_COUNT)]
void gs_main(triangle GS_INPUT input[3], inout TriangleStream<PS_INPUT> outputStream)
{
    float angle = 2.f * M_PI / TEXTURES_IN_ONE_BUSH;
    
    [unroll]
    for (uint i = 0; i < TEXTURES_IN_ONE_BUSH; ++i)
    {
        float angle_cos, angle_sin;
        sincos(angle * i, angle_sin, angle_cos);
        float3x3 rotate_y = float3x3(float3(angle_cos, 0, angle_sin), float3(0, 1, 0), float3(-angle_sin, 0, angle_cos));
        
        [unroll]
        for (uint vertex = 0; vertex < 3; ++vertex)
        {
            PS_INPUT output;
            
            float3 pos = input[vertex].vertex_pos;
            pos = mul(pos, rotate_y);
            output.normal = mul(input[vertex].normal, rotate_y);
            output.tangent = mul(input[vertex].tangent, rotate_y);
            output.bitangent = mul(input[vertex].bitangent, rotate_y);
            pos += input[vertex].grass_pos;
            
            output.world_pos = pos;
            
            output.position = mul(float4(pos, 1), g_viewProj);
            output.tex_coord = input[vertex].tex_coord;
            outputStream.Append(output);
        }
        outputStream.RestartStrip();
    }
}

cbuffer MaterialConstantBuffer : register(b1)
{
    float g_metalnessValue;
    float g_roughnessValue;
    bool g_hasMetalnessMap;
    bool g_hasRoughnessMap;
    
    bool g_hasNormalMap;
    bool g_reversedNormalTextureY;
    float2 paddingMCB;
}

#include "pbr_helpers.hlsli"
#include "environment.hlsli"
#include "opaque_helpers.hlsli"
#include "shadow_helpers.hlsli"

float4 ps_main(PS_INPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    Surface surface;
    View view;
    
    fillSurfaceStructure(surface, input.tex_coord, input.normal, input.tangent, input.bitangent);
    fillViewStructure(view, isFrontFace ? surface.map_normal : -surface.map_normal, input.world_pos);
        
    float shadowFactor;
    float3 hdrColor = float3(0, 0, 0);
    float3 pointToLight;
    float3 transmission = float3(0, 0, 0);
    float3 transmittanceRGB = g_translucency.Sample(g_samplerState, input.tex_coord);
    float NdotL;
    for (uint i = 0; i < g_pointLightNum; ++i)
    {
        pointToLight = g_pointLight[i].position - input.world_pos.xyz;
        NdotL = dot(normalize(pointToLight), surface.map_normal);
        
        transmission = NdotL < 0 ? g_pointLight[i].radiance * transmittanceRGB * pow(-NdotL, 30) : 0;
        
        shadowFactor = calcPointLightShadowFactor(input.world_pos.xyz, surface.map_normal, i);
        hdrColor += (1.f - shadowFactor) * (transmission + CalculatePointLight(g_pointLight[i], pointToLight, view, surface));
    }
    
    for (uint j = 0; j < g_directionalLightNum; ++j)
    {
        NdotL = dot(g_directionalLight[j].direction, surface.map_normal);
        
        transmission = NdotL < 0 ? g_directionalLight[j].radiance * transmittanceRGB * pow(-NdotL, 30) : 0;
        
        shadowFactor = calcDirectionalLightShadowFactor(input.world_pos.xyz, view.PointToCameraNormalized, j);
        hdrColor += (1.f - shadowFactor) * (transmission + CalculateDirectionalLight(g_directionalLight[j], view, surface));
    }
    hdrColor += addEnvironmentDiffuseReflection(surface);
    hdrColor *= g_AOMap.Sample(g_samplerState, input.tex_coord);
    return float4(hdrColor, g_opacityMap.Sample(g_samplerState, input.tex_coord));
}

