#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"
// CS_HiZOcclusionTest.hlsl -- AABB를 Hi-Z 피라미드로 테스트하여 visibility 판정

struct AABB_GPU
{
    float3 center;
    float pad0;
    float3 extents;
    float pad1;
};

StructuredBuffer<AABB_GPU> g_AABBs : register(t0);
Texture2D<float> g_HiZTexture : register(t1);
RWStructuredBuffer<uint> g_VisibilityResults : register(u0);

cbuffer CB_OcclusionTest : register(b13)
{
    float4x4 g_ViewProj;
    float2 g_HiZSize; // mip 0 해상도 (1600 X 900)
    uint g_NumObjects;
    uint g_MaxMipLevel;
};

[numthreads(64, 1, 1)]
void CS_MAIN(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= g_NumObjects)
        return;

    AABB_GPU aabb = g_AABBs[index];

    // AABB 8개 꼭짓점을 screen space로 투영
    float2 screenMin = float2(1.0f, 1.0f);
    float2 screenMax = float2(0.0f, 0.0f);
    float minZ = 1.0f;

    [unroll]
    for (uint i = 0; i < 8; ++i)
    {
        // i에 따라 변화
        // 0 : (-1, -1, -1), 1 : (1, -1, -1),
        // 2 : (-1,  1, -1), 3 : (1,  1, -1),
        // 4 : (-1, -1,  1), 5 : (1, -1,  1), 
        // 6 : (-1,  1,  1), 7 : (1,  1,  1)
        float3 corner = aabb.center + aabb.extents * float3(
            (i & 1) ? 1.0f : -1.0f,
            (i & 2) ? 1.0f : -1.0f,
            (i & 4) ? 1.0f : -1.0f
        );

        float4 clipPos = mul(float4(corner, 1.0f), g_ViewProj);

        // 카메라 뒤에 있는 꼭짓점 → 보수적으로 visible 처리
        if (clipPos.w <= 0.0f)
        {
            g_VisibilityResults[index] = 1;
            return;
        }

        float3 ndc = clipPos.xyz / clipPos.w;

        // NDC(-1~1) → UV(0~1) 변환
        float2 uv = float2(ndc.x * 0.5f + 0.5f, -ndc.y * 0.5f + 0.5f);

        // min((1, 1), ?)
        screenMin = min(screenMin, uv);
        screenMax = max(screenMax, uv);
        minZ = min(minZ, ndc.z);
    }

    // 화면 밖 clamp
    float2 margin = 0.05f;
    screenMin = saturate(screenMin - margin);
    screenMax = saturate(screenMax + margin);

    // 카메라 앞쪽 near plane보다 가까우면 무조건 visible
    if (minZ <= 0.0f)
    {
        g_VisibilityResults[index] = 1;
        return;
    }

    // screen rect 크기로 mip level 선택
    float2 rectSizePixels = (screenMax - screenMin) * g_HiZSize;
    // ceil == 내림
    float mipLevel = ceil(log2(max(rectSizePixels.x, rectSizePixels.y))) + 1.f;
    mipLevel = clamp(mipLevel, 0.0f, (float) g_MaxMipLevel);

    // Hi-Z에서 depth 샘플 (5점: 중심 + 4코너, 보수적 판정)
    float2 centerUV = (screenMin + screenMax) * 0.5f;

    float d0 = g_HiZTexture.SampleLevel(PointSampler, centerUV, mipLevel);
    float d1 = g_HiZTexture.SampleLevel(PointSampler, screenMin, mipLevel);
    float d2 = g_HiZTexture.SampleLevel(PointSampler, screenMax, mipLevel);
    float d3 = g_HiZTexture.SampleLevel(PointSampler, float2(screenMax.x, screenMin.y), mipLevel);
    float d4 = g_HiZTexture.SampleLevel(PointSampler, float2(screenMin.x, screenMax.y), mipLevel);

    float maxHiZ = max(max(d0, d1), max(max(d2, d3), d4));

    // minZ > maxHiZ → 완전히 가려짐 (occluded)
    g_VisibilityResults[index] = (minZ > maxHiZ) ? 0 : 1;
}