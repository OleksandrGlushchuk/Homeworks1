#include "globals.hlsli"
#include "gbuffer_unpack.hlsli"
#include "pbr_helpers.hlsli"
#include "shadow_helpers.hlsli"

struct VS_INPUT
{
    float4 viewProj_x : VIEWPROJ_X;
    float4 viewProj_y : VIEWPROJ_Y;
    float4 viewProj_z : VIEWPROJ_Z;
    float4 viewProj_w : VIEWPROJ_W;
};

struct PS_INPUT
{
    float4 position : SV_Position;
    nointerpolation uint dl_index : DL_INDEX;
    float2 tex_coord : TEXCOORD;
    nointerpolation float1 padding : PADDING;
    nointerpolation float4x4 viewProjDirectionalLight : DL_MATRIX;
};

PS_INPUT vs_main(VS_INPUT input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    PS_INPUT output;
    if (vertexID == 0)
    {
        output.position = float4(-1, -1, 0, 1);
        output.tex_coord = float2(0, 1);
    }
    else if (vertexID == 1)
    {
        output.position = float4(-1, 3, 0, 1);
        output.tex_coord = float2(0, -1);
    }
    else if (vertexID == 2)
    {
        output.position = float4(3, -1, 0, 1);
        output.tex_coord = float2(2, 1);
    }
    output.dl_index = instanceID;
    output.viewProjDirectionalLight = float4x4(input.viewProj_x, input.viewProj_y, input.viewProj_z, input.viewProj_w);
    return output;
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    Surface surface;
    float3 world_pos;
    UnpackGbuffer(input.position.xy, world_pos, surface);
    surface.map_normal = surface.isFrontFace ? surface.map_normal : -surface.map_normal;
    View view;
    fillViewStructure(view, surface.map_normal, world_pos.xyz);
    
    float3 transmittanceRGB = surface.emission;
    float NdotL = dot(g_directionalLight[input.dl_index].direction, surface.map_normal);
    float3 transmission = NdotL < 0 ? g_directionalLight[input.dl_index].radiance * transmittanceRGB * pow(-NdotL, TRANSLUCENCY_POWER) : 0;
    
    float shadowFactor;
    float3 hdrColor = float3(0, 0, 0);
    
    shadowFactor = calcDirectionalLightShadowFactor(world_pos.xyz, input.dl_index, g_directionalLight[input.dl_index].direction, input.viewProjDirectionalLight);
    hdrColor += (1.f - shadowFactor) * (transmission + CalculateDirectionalLight(g_directionalLight[input.dl_index], view, surface));
    return float4(hdrColor, 1.f);
}