#pragma pack_matrix(row_major)
#include "Shader_Buffer.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
   
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
   
    Out.vPosition = vPosition;
   
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;

    float4 color = { 0.f, 0.f, 0.f, 0.f };
    if (any(g_vColor))
    {
        color = g_vColor;
    }
    else
    {
        color.r = 1.f;
        color.g = 1.f;
        color.a = 1.f;
    }
    Out.vColor = color;
   
    return Out;
}
