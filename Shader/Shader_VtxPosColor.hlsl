// Shader_VtxPosTex.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// Texture2D + SamplerState 분리해서 선언
Texture2D<float4> g_Texture : register(t0);
Texture2D<float4> g_TextureMask[2] : register(t1);





// 이렇게 해도 되고
//SamplerState DefaultSampler : register(s0)
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = WRAP;
//    AddressV = WRAP;
//};

// 이렇게 해도 된다
// SamplerState LinearSampler : register(s0);

// 단, 위의 방식을 사용할때는 Sampler클래스를 생성할때 들어간 값과 완전히 동일해야 한다

struct VS_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
   
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
   
    Out.vPosition = vPosition;
    Out.vColor = In.vColor;
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

float4 PS_MAIN(PS_IN In) : SV_Target0
{
   return In.vColor;
}


float4 PS_SELECT(PS_IN In) :SV_Target0
{
    return float4(1.f, 0.f, 0.f, 1.f);
    
}
