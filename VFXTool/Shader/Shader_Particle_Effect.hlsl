#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"
// Shader_Particle_Effect.hlsl

Texture2D<float4> g_Texture : register(t0);
float g_vAtlasSize;

struct VS_IN
{
    float3 vPosition : POSITION;
    float4x4 TransformMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float fFrameIndex : TEXCOORD1;
    float fRotation : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4 vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    Out.vPosition = mul(vPosition, g_WorldMatrix);

    // 파티클 크기
    Out.vPSize = float2(length(In.TransformMatrix._11_12_13), length(In.TransformMatrix._21_22_23));

    Out.vLifeTime = In.vLifeTime;
    
    // 프레임 인덱스: TransformMatrix._14 (vRight.w)
    Out.fFrameIndex = In.TransformMatrix._14;
    
    // 회전값: TransformMatrix._24 (vUp.w)
    Out.fRotation = In.TransformMatrix._24;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float fFrameIndex : TEXCOORD1;
    float fRotation : TEXCOORD2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float fFrameIndex : TEXCOORD2;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];

    // 빌보드 방향 계산
    float3 vLook = normalize(g_vCamPosition.xyz - In[0].vPosition.xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y;

    // 회전 적용
    float cosR = cos(In[0].fRotation);
    float sinR = sin(In[0].fRotation);

    float3 rotatedRight = vRight * cosR - vUp * sinR;
    float3 rotatedUp = vRight * sinR + vUp * cosR;

    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);

    // 회전된 방향으로 쿼드 생성
    Out[0].vPosition = mul(float4(In[0].vPosition.xyz + rotatedRight + rotatedUp, 1.f), matVP);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].fFrameIndex = In[0].fFrameIndex;

    Out[1].vPosition = mul(float4(In[0].vPosition.xyz - rotatedRight + rotatedUp, 1.f), matVP);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].fFrameIndex = In[0].fFrameIndex;

    Out[2].vPosition = mul(float4(In[0].vPosition.xyz - rotatedRight - rotatedUp, 1.f), matVP);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].fFrameIndex = In[0].fFrameIndex;

    Out[3].vPosition = mul(float4(In[0].vPosition.xyz + rotatedRight - rotatedUp, 1.f), matVP);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].fFrameIndex = In[0].fFrameIndex;

    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float fFrameIndex : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    int frameIndex = (int) In.fFrameIndex;
    float2 frameSize = 1.f / g_vAtlasSize;

    float2 frameOffset;
    frameOffset.x = (frameIndex % (int) g_vAtlasSize.x) * frameSize.x;
    frameOffset.y = (frameIndex / (int) g_vAtlasSize.x) * frameSize.y;

    float2 atlasUV = frameOffset + In.vTexcoord * frameSize;
    
    float4 vTexColor = g_Texture.Sample(LinearSampler, atlasUV);
    
    float lifeRatio = In.vLifeTime.x / In.vLifeTime.y;
    float fadeAlpha = 1.f - lifeRatio;
    
    Out.vColor = vTexColor * g_vEffectColor;
    Out.vColor.a *= fadeAlpha;
    
    if (Out.vColor.a < 0.01f)
        discard;

    return Out;
}
