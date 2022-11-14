#ifndef _GBUFFER_UNPACK_HLSLI_
#define _GBUFFER_UNPACK_HLSLI_
#include "globals.hlsli"
#include "pbr_helpers.hlsli"
#include "octahedron.hlsli"

Texture2D<float4> g_normals : register(t0);
Texture2D<float4> g_albedo : register(t1);
Texture2D<float4> g_rmf : register(t2);
Texture2D<float4> g_emission : register(t3);
Texture2D<float> g_depth : register(t4);

void UnpackGbuffer(in float2 screenXY, out float3 world_pos, out Surface surface)
{
    float3 sample_location = float3(screenXY, 0);
    float depth = g_depth.Load(sample_location);
    float4 clip_space_pos;
    clip_space_pos.x = screenXY.x / (g_screenWidth / 2.f) - 1.f;
    clip_space_pos.y = -screenXY.y / (g_screenHeight / 2.f) + 1.f;
    clip_space_pos.z = depth;
    clip_space_pos.w = 1.f;
    
    float4 world_pos4 = mul(clip_space_pos, g_viewProjInv);
    world_pos4 /= world_pos4.w;
    
    world_pos = world_pos4.xyz;
    
    float3 rmf = g_rmf.Load(sample_location).xyz;
    
    surface.albedo = g_albedo.Load(sample_location);
    surface.roughness = rmf[0];
    surface.metalness = rmf[1];
    surface.isFrontFace = rmf[2];
    surface.F0 = lerp(INSULATOR_F0, surface.albedo, surface.metalness);
    
    float4 normals = g_normals.Load(sample_location);
    surface.map_normal = unpackOctahedron(normals.xy);
    surface.geometry_normal = unpackOctahedron(normals.zw);
    
    surface.emission = g_emission.Load(sample_location).xyz;
}



#endif