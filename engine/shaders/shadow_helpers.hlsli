#ifndef _SHADOW_HELPERS_HLSLI_
#define _SHADOW_HELPERS_HLSLI_
#include "globals.hlsli"
static const float SHADOW_DEPTH_OFFSET = 0.025f;

TextureCubeArray<float> g_pointLightShadows : register(t8);
Texture2DArray<float> g_directionalLightShadows : register(t9);

cbuffer PointLightDSResolutionBuffer : register(b8)
{
    float g_pointLightDSResolution;
    float3 paddingPLDSRB;
}

uint selectCubeFace(float3 unitDir)
{
    float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
    uint index = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
    return index + (asuint(unitDir[index / 2]) >> 31);
}

float calcPointLightShadowFactor(float3 objectWorldPos, float3 map_normal, uint pointLightIndex, float3 pointLightPosition, float4x4 viewProjPointLightMatrices[6])
{
    float3 toObject = objectWorldPos - pointLightPosition;
    
    float3 toObjectNormalized = normalize(toObject);
    
    uint cubeFaceIndex = selectCubeFace(toObjectNormalized);

    objectWorldPos += (-toObjectNormalized * SHADOW_DEPTH_OFFSET);
    
    float4 posInLightClipSpace = mul(float4(objectWorldPos, 1.f), viewProjPointLightMatrices[cubeFaceIndex]);
    
    float comaringDepth = posInLightClipSpace.z / posInLightClipSpace.w;
    
    float linearDepth = posInLightClipSpace.w;
    
    float texelSize = linearDepth * 2.f / g_pointLightDSResolution;
    
    float3 normal_offset = map_normal * texelSize;
    
    float4 sampleLocation = float4(toObject + normal_offset, pointLightIndex);
    
    return g_pointLightShadows.SampleCmp(g_samplerComparisonState, sampleLocation, comaringDepth);
}

float calcDirectionalLightShadowFactor(float3 objectWorldPos, uint directionalLightIndex, float3 dirLightDirection, float4x4 viewProjDirectionalLight)
{
    objectWorldPos += dirLightDirection * 20 * SHADOW_DEPTH_OFFSET;
    
    float4 posInLightSpace = mul(float4(objectWorldPos, 1.f), viewProjDirectionalLight);

    float depthInLightSpace = posInLightSpace.z;
    
    if (depthInLightSpace < 0)
        return 0;
    
    posInLightSpace.xy = (posInLightSpace).xy * 0.5f + 0.5f;
    posInLightSpace.y = -posInLightSpace.y;

    float3 sampleLocation = float3(posInLightSpace.xy, directionalLightIndex);

    return g_directionalLightShadows.SampleCmp(g_samplerComparisonState, sampleLocation, depthInLightSpace);
}
#endif