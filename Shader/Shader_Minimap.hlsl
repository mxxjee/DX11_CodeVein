// Shader_VtxPosTex.hlsl
#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// Texture2D + SamplerState 분리해서 선언
Texture2D g_DebugTexture : register(t0);
Texture2D g_IconTexture : register(t1);
Texture2D g_AccumulateTexture : register(t2);
Texture2D g_TerrainTexture : register(t3);





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

VS_OUT VS_DEBUG(VS_IN In)
{
    VS_OUT Out;
   
    float4x4 matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
   
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
   
    return Out;
}

VS_OUT VS_STAMP(VS_IN In)
{
    VS_OUT Out;

    float2 vPos = In.vPosition.xy * g_fIconScale;
    
    float fSin, fCos;
    sincos(g_fRotation, fSin, fCos);
    float2 vRotatedPos;
    vRotatedPos.x = vPos.x * fCos - vPos.y * fSin ;
    
    //왜반대로지?
    vRotatedPos.x *= -1.f;
    
    vRotatedPos.y = vPos.x * fSin + vPos.y * fCos;
   // vRotatedPos.y *= -1.f;
    
    float2 vTranslation;
    vTranslation.x = g_vTargetUV.x * 2.f - 1.f;
    vTranslation.y = (g_vTargetUV.y * 2.f - 1.f) * -1.f;
    
    float4 vWorldPos = mul(float4(vRotatedPos + vTranslation, 0.f, 1.f), g_WorldMatrix);
    
    Out.vPosition = mul(vWorldPos, mul(g_ViewMatrix, g_ProjMatrix));
    Out.vTexcoord = In.vTexcoord;
    return Out;
}

VS_OUT VS_TERRAIN(VS_IN In)
{
    VS_OUT Out;
    
    Out.vPosition = float4(In.vPosition.x * 2.f, In.vPosition.y * 2.f, 0.f, 1.0f);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

VS_OUT VS_ARROW(VS_IN In)
{
    VS_OUT Out;

     float2 vPos = In.vPosition.xy * g_fIconScale;
    
      float fSin, fCos;
    sincos(g_fRotation, fSin, fCos);
    
    float2 vRotatedPos;
    vRotatedPos.x = vPos.x * fCos - vPos.y * fSin;
    vRotatedPos.y = vPos.x * fSin + vPos.y * fCos;

     float2 vTranslation;
    vTranslation.x = g_vTargetUV.x * 2.f - 1.f;
    vTranslation.y = (g_vTargetUV.y * 2.f - 1.f) * -1.f;
    Out.vPosition = float4(vRotatedPos + vTranslation, 0.f, 1.0f);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}
////////////////////////////////////////////////////////////////////////
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

PS_OUT PS_DEBUG(PS_IN In)
{
    PS_OUT Out;


    Out.vColor = g_DebugTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor += float4(1.f, 0.f, 1.f, 1.f);
    return Out;
}

PS_OUT PS_STAMP(PS_IN In)
{
    PS_OUT Out;
  
    Out.vColor = g_IconTexture.Sample(LinearSampler, In.vTexcoord);
    if(Out.vColor.a<0.2f)
        discard;
   
    if (g_isClipping)
    {
        if (g_vTargetUV.x <= 0.3f || g_vTargetUV.x >= 0.73f ||
        g_vTargetUV.y <= 0.3f || g_vTargetUV.y >= 0.73f)
        {
            discard;
        }


    }
 

    return Out;
}

PS_OUT PS_Arrow(PS_IN In)
{
    PS_OUT Out;
  
    Out.vColor = g_IconTexture.Sample(LinearSampler, In.vTexcoord);
  
 

    return Out;
}

PS_OUT PS_TERRAIN(PS_IN In) : SV_Target
{
    PS_OUT Out;
    float4 vTerrain = g_TerrainTexture.Sample(LinearSampler, In.vTexcoord + g_MinimapOffSet);
    
    //trace기록한 RTV
    float4 vTrace = g_AccumulateTexture.Sample(LinearSampler, In.vTexcoord);
    
    //합성
    Out.vColor = vTerrain + vTrace;
    return Out;
}