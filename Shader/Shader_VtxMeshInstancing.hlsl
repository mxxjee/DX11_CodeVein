#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// 텍스처 전역 변수
Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_AlphaTexture : register(t1);
Texture2D<float4> g_NormalTexture : register(t2);
Texture2D<float4> g_OpacityTexture : register(t3);

struct VS_IN_INSTANCING
{

    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    
    row_major float4x4 WorldMatrix : WORLD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN_INSTANCING(VS_IN_INSTANCING In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float4 vWorldPos = mul(float4(In.vPosition, 1.f), In.WorldMatrix);
    
    Out.vWorldPos = vWorldPos;
    Out.vPosition = mul(vWorldPos, g_ViewMatrix); // View 변환
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix); // Proj 변환
    
    Out.vProjPos = Out.vPosition;
    
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), In.WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), In.WorldMatrix));
    Out.vBinormal = normalize(mul(float4(In.vBinormal, 0.f), In.WorldMatrix));
    
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    vector vPick : SV_TARGET3;
};

PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID);

    return Out;
}

PS_OUT PS_MAIN_NORMAL(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    float4 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 TBN = float3x3(In.vTangent.xyz, In.vBinormal.xyz * -1.f, In.vNormal.xyz);
    
    vNormal = mul(vNormal, TBN);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID);

    return Out;
}

PS_OUT PS_MAIN_OPACITY(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vMtrlOpacity = g_OpacityTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a <= 0.05f)
        discard;
    if (vMtrlOpacity.r <= 0.5f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse + vMtrlOpacity;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.8f, 400.f);
    Out.vPick = vector(In.vWorldPos.xyz, g_ObjectID);
    
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(VS_OUT In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    return Out;
}