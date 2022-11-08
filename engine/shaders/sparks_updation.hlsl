#include "globals.hlsli"

cbuffer SparksBufferSize : register(b2)
{
    uint g_sparksBufferSize;
    float3 paddingSBS;
};

struct Particle
{
    float3 position;
    float spawnTime;
    float3 velocity;
    float padding;
};

RWStructuredBuffer<Particle> particlesData : register(u0);
RWBuffer<uint> particlesRange : register(u1);
static const float MAX_PARTICLE_LIFETIME = 3.f;

#include "octahedron.hlsli"
Texture2D<float> g_depth : register(t0);
Texture2D<float4> g_normals : register(t1);

[numthreads(64, 1, 1)]
void cs_main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    if (index >= particlesRange[0] + particlesRange[1] || index < particlesRange[0])
        return;
    float currentLifeTime = g_time - particlesData[index].spawnTime;
    if (currentLifeTime > MAX_PARTICLE_LIFETIME)
    {
        InterlockedAdd(particlesRange[2], 1);
        return;
    }
    particlesData[index].velocity.y -= 0.005f;
    particlesData[index].position += particlesData[index].velocity;
    o
    float4 clip_space_pos = mul(float4(particlesData[index].position, 1), g_viewProj);
    clip_space_pos /= clip_space_pos.w;
    
    float texX = (clip_space_pos.x + 1.f) * 0.5f;
    float texY = (clip_space_pos.y - 1.f) * (-0.5f);
    
    int3 sample_location = int3(texX * g_screenWidth, texY * g_screenHeight, 0);
    
    float depth = g_depth.Load(sample_location);
    if (clip_space_pos.z < depth)
    {
        float3 geometry_normal = unpackOctahedron(g_normals.Load(sample_location).zw);
        particlesData[index].position += geometry_normal*0.01f;
        particlesData[index].velocity = geometry_normal * particlesData[index].velocity / 2.f;
        
    }
}