// Shader_AnimMesh.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_NormalTexture : register(t1);
Texture2D<float4> g_RoughTexture : register(t2);

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
    uint4 vBlendIndices : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    float fWeightW = 1.f - ((In.vBlendWeights.x) + (In.vBlendWeights.y) + (In.vBlendWeights.z));
    
    float4x4 BoneMatrix = (g_BonesMatrices[In.vBlendIndices.x] * In.vBlendWeights.x)
                        + (g_BonesMatrices[In.vBlendIndices.y] * In.vBlendWeights.y)
                        + (g_BonesMatrices[In.vBlendIndices.z] * In.vBlendWeights.z)
                        + (g_BonesMatrices[In.vBlendIndices.w] * fWeightW);
    
    //float4 vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);
    //
    //float4x4 matWV, matWVP;
    //matWV = mul(g_WorldMatrix, g_ViewMatrix);
    //matWVP = mul(matWV, g_ProjMatrix);
    //
    //Out.vPosition = mul(vPosition, matWVP);
    //Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);
    //Out.vTexcoord = In.vTexcoord;
    //Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    float4 Position = mul(float4(In.vPosition, 1.f), BoneMatrix);
    float4 vNormal = mul(float4(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(Position, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vMtrlDiffuse;
    
    return Out;
}

struct PS_OUT_SHADOW
{
    float4 vDepth : SV_TARGET0;
};

PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN In)
{
    PS_OUT_SHADOW Out;
    
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    //Out.vDepth = float4(1.f, 0.f, 0.f, 1.f);
    
    return Out;
}