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
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
   
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
   
    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
   
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float3 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearClampSampler, In.vTexcoord);

    if (vColor.a == 0.f)
        discard;
    
    Out.vColor = vColor;
    
    if (g_vColor.r >= 1.1f && g_vColor.g >= 1.1f && g_vColor.b >= 1.1f)
        Out.vColor.a = g_vColor.a;
    else
        Out.vColor.a = 1.f;
    
    return Out;
}

PS_OUT PS_MAIN_COMMNET(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
       
    Out.vColor = vColor;
    Out.vColor.a *= 0.6f;
    
    return Out;
}

PS_OUT PS_MAIN_MASKING(PS_IN In)
{
    PS_OUT Out;
   
    float4 vSrc = g_TextureMask[0].Sample(LinearClampSampler, In.vTexcoord);
    float4 vDest = g_TextureMask[1].Sample(LinearClampSampler, In.vTexcoord);

    float4 vColor = lerp(vSrc, vDest, g_fProgress);
       
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_MAIN_COLOR_MUL(PS_IN In)
{
    PS_OUT Out;
   
    float4 vTexColor = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    
    Out.vColor.rgb = vTexColor.rgb * g_vColor.rgb;
    Out.vColor.a = vTexColor.a * g_vColor.a;
    
    return Out;
}

PS_OUT PS_NO_DISCARD(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vColor;
    
    return Out;
}
