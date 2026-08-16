#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_Texture : register(t0);
Texture2D<float4> g_Texture_Noise : register(t1);
Texture2D<float4> g_Texture_Scene : register(t2);
Texture2D<float4> g_Texture_Mask : register(t3);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float  fAlpha : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float  fAlpha : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

/* 정점 셰이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    // 넘어오는 정점이 이미 월드 좌표이다. 따라서 뷰, 투영만 곱한다.
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    Out.vPosition = mul(float4(In.vPosition, 1.f), matVP);
    Out.vTexcoord = In.vTexcoord;
    Out.fAlpha = In.fAlpha;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float fAlpha : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* 픽셀 셰이더 */
// Pass 0 : 텍스처 + 컬러 틴트 + 알파 페이드
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    float4 vTexColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor = vTexColor * g_vColor;
    Out.vColor.a *= In.fAlpha;
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    if (g_bUseMask)
    {
        float fMask = g_Texture_Mask.Sample(LinearSampler, In.vTexcoord).r;
        Out.vColor.a *= fMask;
    }
    
        return Out;
    }

// Pass 1 : Trail 왜곡
PS_OUT PS_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    // 노이즈 맵 UV에 시간을 더해서 스크롤
    float2 vNoiseUV = In.vTexcoord + float2(g_fTransparency * 0.3f, g_fTransparency * 0.15f);
    
    // 노이즈 맵에서 RG 채널 읽어오기
    float2 vNoise = g_Texture_Noise.Sample(LinearSampler, vNoiseUV).rg;
    
    // 0~1 => -1~1로 변환 후 강도/페이드 적용
    float2 vOffset = (vNoise - 0.5f) * 2.f * g_fDistortionStrength * In.fAlpha;
    
    // 왜곡된  UV로 기본 텍스처 샘플링
    float4 vTexcolor = g_Texture.Sample(LinearSampler, In.vTexcoord + vOffset);
    
    Out.vColor = vTexcolor * g_vColor;
    Out.vColor.a *= In.fAlpha;
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}

// Pass 2 : 화면 왜곡
PS_OUT PS_SCENE_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    // 화면 텍스처의 UV 계산 (투영 좌표 => 0~1범위)
    float2 vScreenUV;
    vScreenUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vScreenUV.y = -In.vProjPos.y / In.vProjPos.w * 0.5f + 0.5f;
    
    // 노이즈 맵으로 왜곡 오프셋 계산
    float2 vNoiseUV = In.vTexcoord + float2(g_fTransparency * 0.3f, g_fTransparency * 0.1f);        // g_fTransparency는 누적 시간을 넘겨주는 중
    float2 vNoise = g_Texture_Noise.Sample(LinearSampler, vNoiseUV).rg;
    float2 vOffset = (vNoise - 0.5f) * 2.f * g_fDistortionStrength * In.fAlpha;
    
    // 왜곡된 UV로 최종 장면 샘플링
    float4 vFinalScene = g_Texture_Scene.Sample(LinearClampSampler, vScreenUV + vOffset);
    
    Out.vColor = vFinalScene * g_vColor;
    Out.vColor.a = In.fAlpha;
    
    return Out;
}

// Pass 3 : 솔리드 컬러 (텍스처 없이, 디버그)
PS_OUT PS_SOLID(PS_IN In)
{
    PS_OUT Out;
    
    Out.vColor = g_vColor;
    Out.vColor.a *= In.fAlpha;
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}