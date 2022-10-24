#ifndef _WIND_HLSLI_
#define _WIND_HLSLI_
#include "globals.hlsli"
static const float2 WIND_DIR = normalize(float2(-1,1));
float computeGrassAngle(float2 instancePos, float2 windDir) // windDir may be read from 2x2 wind matrix
{
    const float MAX_VARIATION = M_PI;
    float WIND_WAVE_LENGTH = 50;
    float POWER_WAVE_LENGTH = 2;
    const float WIND_OSCILLATION_FREQ = 0.666;
    const float POWER_OSCILLATION_FREQ = 1.0 / 8.0;

    float instanceRandom = frac(instancePos.x * 12345);
    float windCoord = dot(instancePos, windDir);
	
    float windPhaseVariation = instanceRandom * MAX_VARIATION;
    float windPhaseOffset = windCoord / WIND_WAVE_LENGTH + windPhaseVariation;
    float windOscillation = (sin(windPhaseOffset - WIND_OSCILLATION_FREQ * M_2PI * g_time) + 1) / 2;
	
    float powerPhaseOffset = windCoord / POWER_WAVE_LENGTH;
    float powerOscillation = (sin(powerPhaseOffset - POWER_OSCILLATION_FREQ * M_2PI * g_time) + 1) / 2;
	
    float minAngle = lerp(0.0, 0.3, powerOscillation);
    float maxAngle = lerp(0.1, 1.0, powerOscillation);
    return lerp(minAngle, maxAngle, windOscillation);
}

#endif