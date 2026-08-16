#include "Shader_Buffer.hlsli"

cbuffer CB_Particle : register(b8)
{
    float3 g_vPivot;
    float g_fTimeDelta;
    
    uint g_iNumParticles;
    uint g_bIsLoop;
    float g_fAccumulatedTime;
    float g_fShakeStrength;

    float4x4 g_matParticleWorld;
    float4x4 g_matPrevParticleWorld;
    uint g_bFollowParent;
    float g_fConvergeMaxDist;
    float2 _padFollow;
}

// 파티클 데이터 구조체
struct Particle
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    float2 vLifeTime;
    float2 vRotation;
};

// 파티클 초기값 저장용
struct ParticleInit
{
    float4 vInitTranslation;
    float fSpeed;
    float fMaxLifeTime;
    float fGravity;
    float fStartDelay;
};

// 사망 파티클의 행렬 정보들
struct Dead_Particle
{
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
};

// UAV 버퍼 (읽기/쓰기)
RWStructuredBuffer<Particle> g_Particles : register(u0);

// 사망한 파티클의 월드 행렬을 저장하는 Append 버퍼 (SubEmitter 전용)
AppendStructuredBuffer<Dead_Particle> g_DeadParticles : register(u1);

// SRV 버퍼 (초기값 읽기 전용)
StructuredBuffer<ParticleInit> g_InitData : register(t0);

// 입출력 버퍼
// 사용하는 쉐이더가 다르면(CS의 경우 함수가 다름 == 다른 쉐이더임) 레지스터 번호가 같아도 됨
// 근데 어지간하면 같은 hlsl 안에서는 다르게 선언합시다 헷갈리니까
// 한 그룹당 256개 스레드
[numthreads(256, 1, 1)]
void CS_SPREAD(uint3 dispatchID : SV_DispatchThreadID)
{
    uint index = dispatchID.x;
    
    // 범위 체크
    if (index >= g_iNumParticles)
        return;
    
    Particle particle = g_Particles[index];
    ParticleInit initParticle = g_InitData[index];
    
    float fPrevLifeTime = particle.vLifeTime.x;
    particle.vLifeTime.x += g_fTimeDelta;

    if (particle.vLifeTime.x < 0.f)
    {
        g_Particles[index] = particle;
        return;
    }

    if (fPrevLifeTime < 0.f && particle.vLifeTime.x >= 0.f)
    {
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (fPrevLifeTime >= 0.f && g_bFollowParent)
    {
        float3 vDelta = g_matParticleWorld[3].xyz - g_matPrevParticleWorld[3].xyz;
        particle.vTranslation.xyz += vDelta;
    }

    vector vLook = normalize(float4((particle.vTranslation.xyz - g_vPivot), 0.f)) * initParticle.fSpeed;

    particle.vTranslation += vLook * g_fTimeDelta;
    particle.vTranslation.y -= initParticle.fGravity * particle.vLifeTime.x * g_fTimeDelta;

    if (g_bIsLoop && particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        particle.vLifeTime.x = 0.f;
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (g_bIsLoop == 0)    // 반복 파티클이 아닐 때 사망 감지
    {
        float fPrevLifeTime = particle.vLifeTime.x - g_fTimeDelta;
        
        if (fPrevLifeTime < particle.vLifeTime.y && particle.vLifeTime.x > particle.vLifeTime.y)
        {
            // 월드 행렬 구성한 후 Append
            Dead_Particle deadData;
            deadData.vRight = particle.vRight;
            deadData.vUp = particle.vUp;
            deadData.vLook = particle.vLook;
            deadData.vTranslation = particle.vTranslation;
            g_DeadParticles.Append(deadData);
        }
    }
    
    g_Particles[index] = particle;
}

// Pivot 쪽으로 모이는 함수
[numthreads(256, 1, 1)]
void CS_CONVERGE(uint3 dispatchID : SV_DispatchThreadID)
{
    uint index = dispatchID.x;
    
    // 범위 체크
    if (index >= g_iNumParticles)
        return;
    
    Particle particle = g_Particles[index];
    ParticleInit initParticle = g_InitData[index];

    float fPrevLifeTime = particle.vLifeTime.x;
    particle.vLifeTime.x += g_fTimeDelta;

    if (particle.vLifeTime.x < 0.f)
    {
        g_Particles[index] = particle;
        return;
    }

    if (fPrevLifeTime <= 0.f && particle.vLifeTime.x > 0.f)
    {
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (fPrevLifeTime > 0.f && g_bFollowParent)
    {
        float3 vDelta = g_matParticleWorld[3].xyz - g_matPrevParticleWorld[3].xyz;
        particle.vTranslation.xyz += vDelta;
    }

    // 파티클 -> 중심 방향으로 (SPREAD랑 반대 방향)
    float3 vCenter = g_vPivot - particle.vTranslation.xyz;
    float fDist = length(vCenter);

    // 중심에 너무 가까우면 멈춤
    if (fDist > 0.05f)
    {
        float3 vDir = vCenter / fDist;
        // particle.vTranslation.xyz += vDir * initParticle.fSpeed * g_fTimeDelta;
        float fMove = min(initParticle.fSpeed * g_fTimeDelta, fDist);
        particle.vTranslation.xyz += vDir * fMove;
    }

    if (g_bIsLoop && particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        particle.vLifeTime.x = 0.f;
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (g_bIsLoop == 0)    // 반복 파티클이 아닐 때 사망 감지
    {
        float fPrevLifeTime = particle.vLifeTime.x - g_fTimeDelta;
        
        if (fPrevLifeTime < particle.vLifeTime.y && particle.vLifeTime.x > particle.vLifeTime.y)
        {
            // 월드 행렬 구성한 후 Append
            Dead_Particle deadData;
            deadData.vRight = particle.vRight;
            deadData.vUp = particle.vUp;
            deadData.vLook = particle.vLook;
            deadData.vTranslation = particle.vTranslation;
            g_DeadParticles.Append(deadData);
        }
    }
    
    g_Particles[index] = particle;
}


// 먼지 같이 자연스러운 무빙
[numthreads(256, 1, 1)]
void CS_DUST(uint3 dispatchID : SV_DispatchThreadID)
{
    uint index = dispatchID.x;
    
    // 범위 체크
    if (index >= g_iNumParticles)
        return;
    
    Particle particle = g_Particles[index];
    ParticleInit initParticle = g_InitData[index];
    
    float fAccumulatedTime = g_fAccumulatedTime;
    float fSeed = (float)index;
    
    // 파티클 마다 다른 Phase 3개, frac 함수는 소수 부분만 추출한다.
    float fPhase1 = frac(fSeed * 0.7123f) * 6.28f;
    float fPhase2 = frac(fSeed * 0.3917f) * 6.28f;
    float fPhase3 = frac(fSeed * 0.5631f) * 6.28f;
    
    float3 vShake;
    vShake.x = sin(fAccumulatedTime * 1.1f + fPhase1) * cos(fAccumulatedTime * 0.7f + fPhase2);
    vShake.y = cos(fAccumulatedTime * 0.8f + fPhase2) * sin(fAccumulatedTime * 1.3f + fPhase3) + 0.15f;     // 살짝 올라가게
    vShake.z = sin(fAccumulatedTime * 0.9f + fPhase3) * cos(fAccumulatedTime * 1.2f + fPhase1);
    
    // 고주파 미세 떨림 추가
    vShake.x += sin(fAccumulatedTime * 3.7f + fPhase2) * 0.2f;
    vShake.y += cos(fAccumulatedTime * 4.1f + fPhase1) * 0.15f;
    vShake.z += sin(fAccumulatedTime * 3.3f + fPhase3) * 0.2f;
    
    float fPrevLifeTimeDust = particle.vLifeTime.x;
    particle.vLifeTime.x += g_fTimeDelta;

    if (particle.vLifeTime.x < 0.f)
    {
        g_Particles[index] = particle;
        return;
    }

    if (fPrevLifeTimeDust < 0.f && particle.vLifeTime.x >= 0.f)
    {
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (fPrevLifeTimeDust >= 0.f && g_bFollowParent)
    {
        float3 vDelta = g_matParticleWorld[3].xyz - g_matPrevParticleWorld[3].xyz;
        particle.vTranslation.xyz += vDelta;
    }

    particle.vTranslation.xyz += vShake * g_fShakeStrength * g_fTimeDelta;

    if (g_bIsLoop && particle.vLifeTime.x >= particle.vLifeTime.y)
    {
        particle.vLifeTime.x = 0.f;
        particle.vTranslation = mul(initParticle.vInitTranslation, g_matParticleWorld);
    }
    else if (g_bIsLoop == 0)    // 반복 파티클이 아닐 때 사망 감지
    {
        float fPrevLifeTime = particle.vLifeTime.x - g_fTimeDelta;
        
        if (fPrevLifeTime < particle.vLifeTime.y && particle.vLifeTime.x > particle.vLifeTime.y)
        {
            // 월드 행렬 구성한 후 Append
            Dead_Particle deadData;
            deadData.vRight = particle.vRight;
            deadData.vUp = particle.vUp;
            deadData.vLook = particle.vLook;
            deadData.vTranslation = particle.vTranslation;
            g_DeadParticles.Append(deadData);
        }
    }
    
    g_Particles[index] = particle;
}


/* 주의사항 */
/*
되도록이면 32 또는 64의 배수로 쓰레드를 배치할 것
이유
NVIDIA 그래픽카드는 32의 배수로 단위(Warp)를 설정해뒀고
AMD 그래픽카드는 64의 배수로 단위(Wave)를 설정해둠

즉 NVIDIA그래픽카드를 사용하는데 쓰레드를 33개 배치했으면 나머지 31개 쓰레드가 놀게 됨
AMD면 65개 배치했을때 63개가 놀게 됨

그래서 되도록이면 32 또는 64의 배수로 쓰레드 배치할 것
64가 제일 좋을듯

근데 그냥 64나 256 쓰셈 큰건 256 아닌건 64
그룹 수만 늘리면 되니까 고정합시당
왜냐하면 64랑 32 겹치는게 64, 256이라서

*/