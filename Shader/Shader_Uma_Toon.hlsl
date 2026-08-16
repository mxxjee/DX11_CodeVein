// Shader_UmaToon.hlsl
#include "Shader_Buffer.hlsli"

Texture2D g_BaseTex : register(t0); // base
Texture2D g_CtrlTex : register(t1); // ctrl
Texture2D g_DiffTex : register(t2); // diff
Texture2D g_ShadTex : register(t3); // shad_c

SamplerState LinearSampler : register(s0);

cbuffer UmaToonParams : register(b9)
{
    float g_ShadowThreshold; // 0.4 권장
    float g_ShadowSoft; // 0.1 권장
    float g_HighlightPower; // 1.0
    float g_RimPower; // 0.5
    
    float4 g_RimColor; // (1, 1, 1, 1)
    
    float g_HighlightSharpness; // 32.0
    float3 padding;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBiNormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float2 vTexcoord : TEXCOORD0;
    float3 vViewDir : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    // 본 스키닝
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    float4x4 BoneMatrix = (g_BonesMatrices[In.vBlendIndices.x] * In.vBlendWeights.x)
                        + (g_BonesMatrices[In.vBlendIndices.y] * In.vBlendWeights.y)
                        + (g_BonesMatrices[In.vBlendIndices.z] * In.vBlendWeights.z)
                        + (g_BonesMatrices[In.vBlendIndices.w] * fWeightW);
    
    float3 skinnedPos = mul(float4(In.vPosition, 1.f), BoneMatrix).xyz;
    float3 skinnedNor = mul(float4(In.vNormal, 0.f), BoneMatrix).xyz;
    float3 skinnedTan = mul(float4(In.vTangent, 0.f), BoneMatrix).xyz;
    
    float4 worldPos = mul(float4(skinnedPos, 1.f), g_WorldMatrix);
    Out.vPosition = mul(mul(worldPos, g_ViewMatrix), g_ProjMatrix);
    
    Out.vNormal = normalize(mul(float4(skinnedNor, 0.f), g_WorldMatrix).xyz);
    Out.vTangent = normalize(mul(float4(skinnedTan, 0.f), BoneMatrix).xyz);
    Out.vTexcoord = In.vTexcoord;
    Out.vViewDir = normalize(g_vCamPosition.xyz - worldPos.xyz);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float2 vTexcoord : TEXCOORD0;
    float3 vViewDir : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // 텍스처 샘플링
    float4 baseTex = g_BaseTex.Sample(LinearSampler, In.vTexcoord);
    float4 ctrlTex = g_CtrlTex.Sample(LinearSampler, In.vTexcoord);
    float4 diffTex = g_DiffTex.Sample(LinearSampler, In.vTexcoord);
    float4 shadTex = g_ShadTex.Sample(LinearSampler, In.vTexcoord);
    
    // 벡터
    float3 N = normalize(In.vNormal);
    float3 L = normalize(-g_vLightDirection.xyz);
    float3 V = normalize(In.vViewDir);
    float3 H = normalize(L + V);
    
    // ===== 1. diff ↔ shad_c 블렌딩 (핵심) =====
    float NdotL = dot(N, L) * 0.5f + 0.5f;
    
    // base.r을 AO로 활용
    NdotL *= lerp(0.5f, 1.f, baseTex.r);
    
//    float shadowFactor = smoothstep(
//        g_ShadowThreshold - g_ShadowSoftness,
//        g_ShadowThreshold + g_ShadowSoftness,
//        NdotL
//    );
    
    float3 baseColor = lerp(shadTex.rgb, diffTex.rgb, shadowFactor);
    
    // ===== 2. 하이라이트 (ctrl.b 마스크) =====
    float highlightMask = ctrlTex.b;
    float NdotH = saturate(dot(N, H));
    float highlight = pow(NdotH, g_HighlightSharpness);
    highlight *= highlightMask * g_HighlightPower * shadowFactor;
    
    float3 highlightColor = diffTex.rgb * 1.2f * highlight;
    
    // ===== 3. 림라이트 =====
    float rim = pow(1.f - saturate(dot(N, V)), 3.f);
    rim *= g_RimPower * saturate(NdotL + 0.3f);
    
    float3 rimColor = g_RimColor.rgb * rim;
    
    // ===== 최종 =====
    float3 finalColor = baseColor + highlightColor + rimColor;
    
    Out.vColor = float4(finalColor, diffTex.a);
    
    return Out;
}

// ===== 아웃라인 =====
VS_OUT VS_OUTLINE(VS_IN In)
{
    VS_OUT Out;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    float4x4 BoneMatrix = (g_BonesMatrices[In.vBlendIndices.x] * In.vBlendWeights.x)
                        + (g_BonesMatrices[In.vBlendIndices.y] * In.vBlendWeights.y)
                        + (g_BonesMatrices[In.vBlendIndices.z] * In.vBlendWeights.z)
                        + (g_BonesMatrices[In.vBlendIndices.w] * fWeightW);
    
    float3 skinnedPos = mul(float4(In.vPosition, 1.f), BoneMatrix).xyz;
    float3 skinnedNor = normalize(mul(float4(In.vNormal, 0.f), BoneMatrix).xyz);
    
    // 노멀 방향으로 확장
    float outlineWidth = 0.001f;
    skinnedPos += skinnedNor * outlineWidth;
    
    float4 worldPos = mul(float4(skinnedPos, 1.f), g_WorldMatrix);
    Out.vPosition = mul(mul(worldPos, g_ViewMatrix), g_ProjMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = float3(0, 0, 0);
    Out.vTangent = float3(0, 0, 0);
    Out.vViewDir = float3(0, 0, 0);
    
    return Out;
}

PS_OUT PS_OUTLINE(PS_IN In)
{
    PS_OUT Out;
    
    float4 shadTex = g_ShadTex.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = float4(shadTex.rgb * 0.3f, 1.f);
    
    return Out;
}
