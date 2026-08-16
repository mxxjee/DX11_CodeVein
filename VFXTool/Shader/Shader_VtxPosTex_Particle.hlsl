#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// Texture2D + SamplerState 분리해서 선언
Texture2D<float4> g_Texture : register(t0);
Texture2D<float4> g_MaskTexture : register(t1);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    if (g_bIsBillboard == 1)
    {
        // 뷰 행렬에서 카메라 축 추출
        float3 camRight = float3(g_ViewMatrix._11, g_ViewMatrix._21, g_ViewMatrix._31);
        float3 camUp = float3(g_ViewMatrix._12, g_ViewMatrix._22, g_ViewMatrix._32);

        // 인스턴스의 스케일 추출 (Right, Up 벡터의 길이)
        float scaleX = length(In.vRight.xyz);
        float scaleY = length(In.vUp.xyz);

        // 빌보드 정점 위치 계산
        float3 localPos = In.vTranslation.xyz
                      + camRight * In.vPosition.x * scaleX
                      + camUp * In.vPosition.y * scaleY;

        float4 worldPos = mul(float4(localPos, 1.f), g_EffectWorldMatrix);
        
        float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
        Out.vPosition = mul(worldPos, matVP);
    }
    else
    {
        float4x4 matTransformationMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
        float4 vPosition = mul(float4(In.vPosition, 1.f), matTransformationMatrix);
        float4 worldPos = mul(vPosition, g_EffectWorldMatrix);
    
        float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);    
        Out.vPosition = mul(worldPos, matVP);
    }    
    
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    // 쓰레기
    Out.vColor.a = vPadding;
    Out.vColor.a = 1.f;
   
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if(vColor.a <= 0.3f)
        discard;
    
    if (g_bUseMask)
        Out.vColor.a *= g_MaskTexture.Sample(LinearSampler, In.vTexcoord).r;

    vColor.a = saturate(In.vLifeTime.y - In.vLifeTime.x);
    Out.vColor = vColor * g_vColor;
   
    return Out;
}

PS_OUT PS_SPRITE(PS_IN In)
{
    PS_OUT Out;
    
    // 쓰레기
    Out.vColor.a = vPadding;
    Out.vColor.a = 1.f;
    
    // LifeTime 기반으로 프레임 계산
    float fProgress = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    // 프레임 인덱스 계산
    uint iFrameIndex = (uint)(fProgress * (float)g_iFrameCount);
    iFrameIndex = min(iFrameIndex, g_iFrameCount - 1);
    
    // UV Offset 계산
    float2 vOffsetUV = float2((float)(iFrameIndex % g_iCountX) * g_vScaleUV.x,
                              (float)(iFrameIndex / g_iCountX) * g_vScaleUV.y);
    
    // 텍스처 샘플링
    vector vColor = g_Texture.Sample(LinearSampler, In.vTexcoord * g_vScaleUV + vOffsetUV);
    
    if (vColor.a <= 0.3f)
        discard;
    
    float fTransparency = 1.f;
    if (iFrameIndex > g_iTransparentIndex && g_iTransparentIndex < g_iFrameCount)
    {
        uint iRemainFrame = g_iFrameCount - g_iTransparentIndex;
        uint iCurrentOffset = iFrameIndex - g_iTransparentIndex;
        fTransparency = 1.f - ((float) (iCurrentOffset + 1) / (float) iRemainFrame);
    }
    
    if (g_bUseMask)
        Out.vColor.a *= g_MaskTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    vColor.rgba *= g_fTransparency * g_vColor;    
    Out.vColor = vColor * g_fAlpha;
    
    return Out;
}
