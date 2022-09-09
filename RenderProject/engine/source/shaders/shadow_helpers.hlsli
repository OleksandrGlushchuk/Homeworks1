#ifndef _SHADOW_HELPERS_HLSLI_
#define _SHADOW_HELPERS_HLSLI_
#include "globals.hlsli"
static const float SHADOW_DEPTH_OFFSET = 0.0002f;
static const float SHADOW_NORMAL_OFFSET = 0.0002f;

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
    
    [unroll]
    for (uint i = 1; i < 6; ++i)
    {
        frustumIndex = dotProduct[i] > dotProduct[frustumIndex] ? i : frustumIndex;
    }
    
    float4 posInLightSpace = mul(float4(objectWorldPos, 1.f), g_viewProjPointLight[pointLightIndex][frustumIndex]);
    return (posInLightSpace.xyz / posInLightSpace.w).z;
}

float calcShadowFactor(float3 objectWorldPos, float3 pointToCameraNormalized, uint pointLightIndex)
{
    float depth_in_light_space = getDepthInLightSpace(objectWorldPos.xyz, pointLightIndex) + SHADOW_DEPTH_OFFSET;
    
    float3 normal_offset = SHADOW_NORMAL_OFFSET * pointToCameraNormalized;
    float3 toObject = normalize(objectWorldPos.xyz - g_pointLight[pointLightIndex].position);
    
    float shadowVal = 0;
    float4 sampleLocation = float4(toObject + normal_offset, pointLightIndex);
    shadowVal = g_shadows.SampleCmp(g_samplerComparisonState, sampleLocation, depth_in_light_space);
    return shadowVal;
}
#endif