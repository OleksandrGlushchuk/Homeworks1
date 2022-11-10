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


#include "octahedron.hlsli"
Texture2D<float> g_depth : register(t0);
Texture2D<float4> g_normals : register(t1);

[numthreads(64, 1, 1)]
void cs_main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;
    if (index >= (particlesRange[1]))
        return;
    index = (index + particlesRange[0]) % g_sparksBufferSize;
    
    
    float currentLifeTime = g_time - particlesData[index].spawnTime;
    if (currentLifeTime > MAX_PARTICLE_LIFETIME)
    {
        InterlockedAdd(particlesRange[2], 1);
        return;
    }
    particlesData[index].velocity.y -= 0.1f * g_deltaTime;
    particlesData[index].position += particlesData[index].velocity;

    float4 clip_space_pos = mul(float4(particlesData[index].position, 1), g_viewProj);
    clip_space_pos /= clip_space_pos.w;
    
    float texX = (clip_space_pos.x + 1.f) * 0.5f;
    float texY = (clip_space_pos.y - 1.f) * (-0.5f);
    
    int3 sample_location = int3(texX * g_screenWidth, texY * g_screenHeight, 0);
    
    float sceneDepth = g_depth.Load(sample_location);
    clip_space_pos.z = sceneDepth;
    float4 scenePos = mul(clip_space_pos, g_viewProjInv);
    scenePos /= scenePos.w;
    sceneDepth = length(scenePos.xyz - g_cameraPos);
    float sparkDepth = length(particlesData[index].position - g_cameraPos);
    
    if (abs(sceneDepth - sparkDepth) < 0.09f)
    {
        float speed = length(particlesData[index].velocity);
        
        float3 normal = unpackOctahedron(g_normals.Load(sample_location).xy);
            
        particlesData[index].velocity = normalize(normal + normalize(particlesData[index].velocity)) * speed * 0.7f;
        particlesData[index].position = scenePos.xyz + particlesData[index].velocity;
    }
}