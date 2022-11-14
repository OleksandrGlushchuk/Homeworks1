cbuffer SparksBufferSize : register(b2)
{
    uint g_sparksBufferSize;
    float3 paddingSBS;
};

struct DrawInstancedIndirectArgs
{
    uint VertexCountPerInstance;
    uint InstanceCount;
    uint StartVertexLocation;
    uint StartInstanceLocation;
};

RWStructuredBuffer<DrawInstancedIndirectArgs> g_args : register(u0);
RWBuffer<uint> particlesRange : register(u1);

[numthreads(1, 1, 1)]
void cs_main( uint3 DTid : SV_DispatchThreadID )
{
    particlesRange[0] = (particlesRange[0] + particlesRange[2]) % g_sparksBufferSize;
    particlesRange[1] = particlesRange[1] - particlesRange[2];
    particlesRange[2] = 0;
    
    uint index = DTid.x;
    
    DrawInstancedIndirectArgs args;
    args.InstanceCount = particlesRange[1];
    args.VertexCountPerInstance = 6;
    args.StartInstanceLocation = 0;
    args.StartVertexLocation = 0;
    
    g_args[index] = args;
}