#ifndef _SHADOW_HELPERS_HLSLI_
#define _SHADOW_HELPERS_HLSLI_
#include "globals.hlsli"
static const float SHADOW_DEPTH_OFFSET = 0.02f;

TextureCubeArray<float> g_pointLightShadows : register(t4);
Texture2DArray<float> g_directionalLightShadows : register(t5);

static const float3 cubeFaceDirections[6] =
{
    float3(1, 0, 0), float3(-1, 0, 0), float3(0, 1, 0), float3(0, -1, 0), float3(0, 0, 1), float3(0, 0, -1)
};

uint selectCubeFace(float3 unitDir)
{
    float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
    uint index = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
    return index + (asuint(unitDir[index / 2]) >> 31);
}

float getDepthInLightSpace(float3 objectWorldPos, uint pointLightIndex, uint cubeFaceIndex)
{
    float4 posInLightSpace = mul(float4(objectWorldPos, 1.f), g_viewProjPointLight[pointLightIndex][cubeFaceIndex]);
    return (posInLightSpace.xyz / posInLightSpace.w).z;
}

float calcPointLightShadowFactor(float3 objectWorldPos, float3 pointToCameraNormalized, uint pointLightIndex)
{
    float3 toObject = objectWorldPos - g_pointLight[pointLightIndex].position;
    
    float3 toObjectNormalized = normalize(toObject);
    
    uint cubeFaceIndex = selectCubeFace(toObjectNormalized);
    
    float depth_in_light_space = getDepthInLightSpace(objectWorldPos + (-toObjectNormalized * SHADOW_DEPTH_OFFSET), pointLightIndex, cubeFaceIndex);
    
    float texelSize = length(toObject) * dot(cubeFaceDirections[cubeFaceIndex], toObjectNormalized) * 2.f / g_pointLightDSResolution;
    float3 normal_offset = pointToCameraNormalized * texelSize;
    
    float4 sampleLocation = float4(toObject + normal_offset, pointLightIndex);
    
    return g_pointLightShadows.SampleCmp(g_samplerComparisonState, sampleLocation, depth_in_light_space);
}

float calcDirectionalLightShadowFactor(float3 objectWorldPos, float3 pointToCameraNormalized, uint directionalLightIndex)
{
    objectWorldPos += g_directionalLight[directionalLightIndex].direction * 20 * SHADOW_DEPTH_OFFSET;
    
    float4 posInLightSpace = mul(float4(objectWorldPos, 1.f), g_viewProjDirectionalLight[directionalLightIndex]);

    float depthInLightSpace = posInLightSpace.z;
    
    if (depthInLightSpace < 0)
        return 0;
    
    posInLightSpace.xy = (posInLightSpace).xy * 0.5f + 0.5f;
    posInLightSpace.y = -posInLightSpace.y;

    float3 sampleLocation = float3(posInLightSpace.xy, directionalLightIndex);

    return g_directionalLightShadows.SampleCmp(g_samplerComparisonState, sampleLocation, depthInLightSpace);
}
#endif