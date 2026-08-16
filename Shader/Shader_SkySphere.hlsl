#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT_SUN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

/* 정점 셰이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition.xyww;
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

VS_OUT_SUN VS_MAIN_SUN(VS_IN In)
{
    VS_OUT_SUN Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vWolrdPos = vPosition;
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN_SUN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWolrdPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vA : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    //float4 vC : SV_TARGET3;
    float4 vPBR : SV_TARGET4;
    //float4 vE : SV_TARGET5;
    //float4 vF : SV_TARGET6;
    float4 vEmmisive : SV_TARGET7;
};

struct PS_OUT_SUN
{
    float4 vColor : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPick : SV_TARGET3;
    float4 vPBR : SV_TARGET4;
    float4 vSpecular : SV_TARGET5;
    float4 vRoughness : SV_TARGET6;
    float4 vEmissive : SV_Target7;
};

/* 픽셀 셰이더 */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vA = DefaultNormal;
    Out.vDepth = float4(0.f, 1.f, 2.f, 0.f); //z값 2면 스카이스피어로구분
    //Out.vC = DefaultPick;
    Out.vPBR = DefaultPBR;
    //Out.vE = DefaultSpecular;
    //Out.vF = DefaultRoughness;
    Out.vEmmisive = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord); //* g_fBambooEmission; //emissive 
    
    return Out;
}

PS_OUT_SUN PS_MAIN_SUN(PS_IN_SUN In)
{
    PS_OUT_SUN Out;
    float4 vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vColor.a <= 0.2)
        vColor = float4(1.0f, 0.7f, 0.3f, 1.f);
    
    Out.vColor = g_vGodRayColor;
    Out.vNormal = DefaultNormal;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 15.f, 400.f);
    Out.vPick = vector(In.vWolrdPos.xyz, g_ObjectID);
    Out.vPBR = DefaultPBR;
    Out.vSpecular = DefaultSpecular;
    Out.vRoughness = DefaultRoughness;
    Out.vEmissive = Out.vColor;
    
    return Out;
}
