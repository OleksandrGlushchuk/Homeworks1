#include "fullscreen.hlsli"
#include "environment.hlsli"
#include "gbuffer_unpack.hlsli"

PS_INPUT vs_main(VS_INPUT input)
{
    return fullscreenVertex(input);
}

float4 ps_main(PS_INPUT input) : SV_Target
{
    Surface surface;
    float3 world_pos;
    
    UnpackGbuffer(input.position.xy, world_pos, surface);
    
    float4 result = float4(0, 0, 0, 1);
    result.xyz = addEnvironmentDiffuseReflection(surface);
    return result;
}