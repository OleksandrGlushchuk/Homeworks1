#ifndef _RESOLVE_HELPERS_HLSLI_
#define _RESOLVE_HELPERS_HLSLI_

inline float3 AdjustExposure(uniform float3 radiance, uniform float _EV100)
{
    float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, _EV100);
    return radiance / LMax;
}

inline float3 ACES(uniform float3 hdr)
{
    float3x3 m1 = float3x3(float3(0.59719f, 0.07600f, 0.02840f), float3(0.35458f, 0.90834f, 0.13383f), float3(0.04823f, 0.01566f, 0.83777f));
    float3x3 m2 = float3x3(float3(1.60475f, -0.10208, -0.00327f), float3(-0.53108f, 1.10813, -0.07276f), float3(-0.07367f, -0.00605, 1.07602f));
    float3 v = mul(hdr, m1);

    float3 a = v * (v + float3(0.0245786f, 0.0245786f, 0.0245786f)) - float3(0.000090537f, 0.000090537f, 0.000090537f);
    float3 b = v * (0.983729f * v + float3(0.4329510f, 0.4329510f, 0.4329510f)) + float3(0.238081f, 0.238081f, 0.238081f);

    float3 ldr = mul(a / b, m2);
    
    ldr = clamp(ldr, 0.0f, 1.0f);
    return ldr;
}

inline float3 GammaCorrection(uniform float3 radiance)
{
    return pow(radiance, 1.0f / 2.2f);
}
#endif