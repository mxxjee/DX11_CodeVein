#pragma pack_matrix(row_major)

cbuffer MatrixBuffer : register(b12)
{
    float4x4 g_WorldMatrix;
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    // 이제 g_WorldMatrix가 정상적으로 들어올 겁니다.
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vColor = float4(In.vColor, 1.f);
    Out.vProjPos = vPosition;
    
    return Out;
}

float4 PS_MAIN(VS_OUT In) : SV_TARGET0
{
    return float4(In.vColor.rgb, 1.f);
}