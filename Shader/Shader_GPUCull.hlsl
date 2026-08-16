// Shader_GPUCull.hlsl
// GPU Frustum Culling - 4 Pass Compute Shader

#include "Shader_Buffer.hlsli"

// ============================================================
//                   구조체 정의
// ============================================================
struct ObjectData
{
    float3 vCenter;
    float fMaxExtent;
    float3 vExtents;
    uint iGroupIndex;
    float4x4 matWorld;
};

struct GroupDesc
{
    uint iFirstObject;
    uint iObjectCount;
    uint iFirstArgsEntry;
    uint iMeshCount;
};

cbuffer CB_GPUCull : register(b11)
{
    float4 g_CullPlanes[6];
    float4 g_CullCamPos;
    uint g_CullNumObjects;
    uint g_CullNumGroups;
    float g_CullThresholdSq;
    uint g_CullPad;
};

// ============================================================
//       통합 리소스 선언 (슬롯 고유 배정)
// ============================================================

// SRV (t 슬롯)
StructuredBuffer<ObjectData> g_Objects : register(t0);
StructuredBuffer<GroupDesc> g_GroupDesc : register(t1);
StructuredBuffer<uint> g_VisibilityIn : register(t2); // Pass 2,4에서 읽기
StructuredBuffer<uint> g_CountRead : register(t3); // Pass 3에서 읽기
StructuredBuffer<uint> g_OffsetRead : register(t4); // Pass 4에서 읽기

// UAV (u 슬롯)
RWStructuredBuffer<uint> g_VisibilityOut : register(u0); // Pass 1에서 쓰기
RWStructuredBuffer<uint> g_CountBuffer : register(u1); // Pass 2,4에서 카운팅
RWStructuredBuffer<uint> g_GroupOffset : register(u2); // Pass 3에서 쓰기
RWByteAddressBuffer g_IndirectArgs : register(u3); // Pass 3에서 쓰기
RWStructuredBuffer<float4x4> g_OutMatrices : register(u4); // Pass 4에서 쓰기


// ============================================================
//               N-Vertex AABB-Frustum 판정
// ============================================================
bool TestAABB_Frustum(float3 _vCenter, float3 _vExtents)
{
    [unroll]
    for (uint i = 0; i < 6; ++i)
    {
        float3 n = g_CullPlanes[i].xyz;
        float d = g_CullPlanes[i].w;

        float3 vN;
        vN.x = (n.x >= 0.0f) ? (_vCenter.x - _vExtents.x) : (_vCenter.x + _vExtents.x);
        vN.y = (n.y >= 0.0f) ? (_vCenter.y - _vExtents.y) : (_vCenter.y + _vExtents.y);
        vN.z = (n.z >= 0.0f) ? (_vCenter.z - _vExtents.z) : (_vCenter.z + _vExtents.z);

        if (dot(n, vN) + d > 0.0f)
            return false;
    }
    return true;
}


// ============================================================
//    Pass 1 : Frustum + Small Object Culling
//    SRV: t0(Objects)
//    UAV: u0(VisibilityOut)
// ============================================================
[numthreads(256, 1, 1)]
void CS_Cull(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= g_CullNumObjects)
        return;

    ObjectData obj = g_Objects[index];

    if (!TestAABB_Frustum(obj.vCenter, obj.vExtents))
    {
        g_VisibilityOut[index] = 0;
        return;
    }

    float3 vDiff = obj.vCenter - g_CullCamPos.xyz;
    float fDistSq = dot(vDiff, vDiff);

    if (obj.fMaxExtent * obj.fMaxExtent < g_CullThresholdSq * fDistSq)
    {
        g_VisibilityOut[index] = 0;
        return;
    }

    g_VisibilityOut[index] = 1;
}


// ============================================================
//    Pass 2 : 그룹별 visible 개수 카운팅
//    SRV: t0(Objects), t2(VisibilityIn)
//    UAV: u1(CountBuffer)
// ============================================================
[numthreads(256, 1, 1)]
void CS_Count(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= g_CullNumObjects)
        return;

    if (g_VisibilityIn[index] == 0)
        return;

    uint groupIndex = g_Objects[index].iGroupIndex;
    InterlockedAdd(g_CountBuffer[groupIndex], 1);
}


// ============================================================
//    Pass 3 : PrefixSum + IndirectArgs 기록
//    SRV: t1(GroupDesc), t3(CountRead)
//    UAV: u2(GroupOffset), u3(IndirectArgs)
// ============================================================
[numthreads(1, 1, 1)]
void CS_BuildArgs(uint3 DTid : SV_DispatchThreadID)
{
    uint offset = 0;

    for (uint g = 0; g < g_CullNumGroups; ++g)
    {
        g_GroupOffset[g] = offset;

        uint visibleCount = g_CountRead[g];
        uint firstArgs = g_GroupDesc[g].iFirstArgsEntry;
        uint meshCount = g_GroupDesc[g].iMeshCount;

        for (uint m = 0; m < meshCount; ++m)
        {
            uint byteOffset = (firstArgs + m) * 20;
            g_IndirectArgs.Store(byteOffset + 4, visibleCount);
            g_IndirectArgs.Store(byteOffset + 16, offset);
        }

        offset += visibleCount;
    }
}


// ============================================================
//    Pass 4 : Visible WorldMatrix Compact 배치
//    SRV: t0(Objects), t2(VisibilityIn), t4(OffsetRead)
//    UAV: u1(CountBuffer), u4(OutMatrices)
// ============================================================
[numthreads(256, 1, 1)]
void CS_Scatter(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x;
    if (index >= g_CullNumObjects)
        return;

    if (g_VisibilityIn[index] == 0)
        return;

    uint groupIndex = g_Objects[index].iGroupIndex;

    uint localIndex;
    InterlockedAdd(g_CountBuffer[groupIndex], 1, localIndex);

    uint outIndex = g_OffsetRead[groupIndex] + localIndex;
    g_OutMatrices[outIndex] = g_Objects[index].matWorld;
}