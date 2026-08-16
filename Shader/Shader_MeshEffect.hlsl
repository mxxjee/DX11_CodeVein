#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

Texture2D<float4> g_DiffuseTexture : register(t0);
Texture2D<float4> g_MaskTexture : register(t1);
Texture2D<float4> g_SceneTexture : register(t2);
Texture2D<float4> g_NoiseTexture : register(t3);
Texture2D<float4> g_AlphaTexture : register(t4);
Texture2D<float4> g_DepthTexture : register(t30);

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vClipPos : TEXCOORD2;
    float4 vPrevClipPos : TEXCOORD3;
};

/* 정점의 기본적인 변환을 가져간다. World, View, Proj */ 
/* 정점의 구성 정보를 변형한다. (멤버를 늘리거나 , 줄이거나 ) */ 

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4 vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    float4 vPrevPosition = mul(float4(In.vPosition, 1.f), g_PrevWorldMatrix);
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    vPrevPosition = mul(vPrevPosition, g_ViewMatrix);
    vPrevPosition = mul(vPrevPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    Out.vClipPos = vPosition;
    Out.vPrevClipPos = vPrevPosition;
    
    return Out;
}

VS_OUT VS_FIRE(VS_IN In)
{
    VS_OUT Out;
    
    // 노이즈로 버텍스 오프셋 계산
    float2 vDisplaceUV = In.vTexcoord + float2(0.f, -g_fTimeElapsed * 0.5f);
    float fDisplace = g_NoiseTexture.SampleLevel(LinearSampler, vDisplaceUV, 0).r;
    
    // 노말 방향으로 밀어내기
    float3 vDisplacedPos = In.vPosition + In.vNormal * fDisplace * g_fDisplaceStrength;

    float4 vPosition = mul(float4(vDisplacedPos, 1.f), g_WorldMatrix);
    float4 vPrevPosition = mul(float4(vDisplacedPos, 1.f), g_PrevWorldMatrix);
    
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);
    
    vPrevPosition = mul(vPrevPosition, g_ViewMatrix);
    vPrevPosition = mul(vPrevPosition, g_ProjMatrix);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    Out.vClipPos = vPosition;
    Out.vPrevClipPos = vPrevPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float4 vClipPos : TEXCOORD2;
    float4 vPrevClipPos : TEXCOORD3;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vEmissive : SV_TARGET1;
    float4 vVelocity : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
   
    float4 vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    // 기본 텍스처
    if (g_bUseTexture)
    {
        float2 vDiffuseUV = In.vTexcoord + g_vDiffuseUVScroll * g_fTimeElapsed;
        vColor = g_DiffuseTexture.Sample(LinearSampler, vDiffuseUV);
        
        if (vColor.a <= 0.2f)
            discard;
    }
    else
    {
        vColor = float4(1.f, 1.f, 1.f, 1.f);
    }

    float fLifeTime = saturate(g_fTimeElapsed / g_fLifeTime); // 0 => 1
    
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fLifeTime / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fLifeTime) / g_vFadeInOut.y) : 1.f;
    vColor.a *= fFadeIn * fFadeOut;
    
    Out.vColor = vColor * g_vColor;
    
    // 알파 마스크
    if (g_bUseAlphaMask)
    {
        float fAlphaMask = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord).r;
        
        Out.vColor.a *= fAlphaMask;
    }

    // 마스크 텍스처 적용
    if (g_bUseMask)
    {
        float2 vMaskUV = In.vTexcoord * g_vMaskUVScale;

        if (g_iMaskFrameCount > 1)
        {
            uint iMaskFrame = (uint) (fLifeTime * (float) g_iMaskFrameCount);
            iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
            iMaskFrame += g_iMaskStartFrame;

            float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
            float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);

            vMaskUV = In.vTexcoord * g_vMaskUVScale * vMaskFrameSize + vMaskOffset;
        }

        vMaskUV += g_vMaskUVStartOffset + g_vMaskUVScroll * g_fTimeElapsed;
        
        float fMask;
        if (g_iMaskSampler == 0)
            fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
        else if (g_iMaskSampler == 1)
            fMask = g_MaskTexture.Sample(BorderSampler, vMaskUV).r;
        
        fMask = saturate(fMask * g_fMaskIntensity);
        Out.vColor.a *= fMask;
    }
    
    // Noise 텍스처
    if (g_bUseNoise)
    {
        float fNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
        
        if (g_bDirectionalDissolve)
        {
            float fDirBias = In.vTexcoord.y;
            float fMask = fNoise * g_fNoiseWeight + fDirBias * (1.f - g_fNoiseWeight);
            float fThreshold = g_bDissolveFlipY ? (1.f - fLifeTime) : fLifeTime;
            
            if (fMask < fThreshold)
                discard;
            
            // 엣지 글로우
            float fEdge = smoothstep(fThreshold, fThreshold + g_fDissolveEdgeWidth, fMask);
            Out.vColor.rgb += (1.f - fEdge) * g_vDissolveEdgeColor;
        }
        else
        {
            if (fNoise < fLifeTime)
                discard;
        
            // 엣지 글로우
            if (g_bUseEdgeGlow)
            {
                float fEdge = smoothstep(fLifeTime, fLifeTime + g_fGlowStrength, fNoise);
                Out.vColor.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
            }
        }
    }
    
    if (g_bEmissive)
        Out.vEmissive = Out.vColor * 2.f;
    else
        Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    //Velocity 계산
    if (Out.vColor.a >= 0.3f)
    {
        float2 curUV = In.vClipPos.xy / In.vClipPos.w;
        curUV = curUV * float2(0.5f, -0.5f) + 0.5f;
    
        float2 prevUV = In.vPrevClipPos.xy / In.vPrevClipPos.w;
        prevUV = prevUV * float2(0.5f, -0.5f) + 0.5f;
    
        Out.vVelocity = float4(curUV - prevUV, 0.f, 1.f);
    }
    else
        Out.vVelocity = float4(0.f, 0.f, 0.f, 1.f);
    
    // Soft Particle
    float2 vScreenUV = In.vProjPos.xy / In.vProjPos.w;
    vScreenUV = vScreenUV * 0.5f + 0.5f;
    vScreenUV.y = 1.f - vScreenUV.y;
    
    float fSceneDepth = g_DepthTexture.Sample(PointSampler, vScreenUV).y;
    if (fSceneDepth < 0.001f)
        return Out;
    
    float fMeshDepth = In.vProjPos.w;
    float fSoftFade = saturate((fSceneDepth - fMeshDepth) / 2.5f);
    Out.vColor.a *= fSoftFade;
    
    return Out;
}

PS_OUT PS_SPRITE(PS_IN In)
{
    PS_OUT Out;
    
    // LifeTime 기반으로 프레임 계산    
    float fLifeTime = saturate(g_fTimeElapsed / g_fLifeTime);

    // 프레임 인덱스 계산
    uint iFrameIndex = (uint) (fLifeTime * (float) g_iFrameCount);
    iFrameIndex = min(iFrameIndex, g_iFrameCount - 1);
    
    // UV Offset 계산
    float2 vOffsetUV = float2((float) (iFrameIndex % g_iCountX) * g_vScaleUV.x,
                              (float) (iFrameIndex / g_iCountX) * g_vScaleUV.y);
    
    // 텍스처 샘플링
    vector vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vScaleUV + vOffsetUV);
    
    if (vColor.a <= 0.2f)
        discard;
    
    float fTransparency = 1.f;
    if (iFrameIndex > g_iTransparentIndex && g_iTransparentIndex < g_iFrameCount)
    {
        uint iRemainFrame = g_iFrameCount - g_iTransparentIndex;
        uint iCurrentOffset = iFrameIndex - g_iTransparentIndex;
        fTransparency = 1.f - ((float) (iCurrentOffset + 1) / (float) iRemainFrame);
    }
    
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fLifeTime / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fLifeTime) / g_vFadeInOut.y) : 1.f;

    vColor.rgba *= fTransparency * g_vColor;
    Out.vColor = vColor * g_fAlpha;
    Out.vColor.a *= fFadeIn * fFadeOut;
    
    // 마스크 텍스처 적용
    if (g_bUseMask)
    {
        float2 vMaskUV = In.vTexcoord * g_vMaskUVScale;

        if (g_iMaskFrameCount > 1)
        {
            uint iMaskFrame = (uint) (fLifeTime * (float) g_iMaskFrameCount);
            iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
            iMaskFrame += g_iMaskStartFrame;

            float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
            float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);

            vMaskUV = In.vTexcoord * g_vMaskUVScale * vMaskFrameSize + vMaskOffset;
        }

        vMaskUV += g_vMaskUVStartOffset + g_vMaskUVScroll * g_fTimeElapsed;

        float fMask;
        if (g_iMaskSampler == 0)
            fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
        else if (g_iMaskSampler == 1)
            fMask = g_MaskTexture.Sample(BorderSampler, vMaskUV).r;
        
        fMask = saturate(fMask * g_fMaskIntensity);
        Out.vColor.a *= fMask;
    }

        // Noise 텍스처
    if (g_bUseNoise)
    {
        float fNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).r;
        
        if (g_bDirectionalDissolve)
        {
            float fDirBias = In.vTexcoord.y;
            float fMask = fNoise * g_fNoiseWeight + fDirBias * (1.f - g_fNoiseWeight);
            float fThreshold = g_bDissolveFlipY ? (1.f - fLifeTime) : fLifeTime;
            
            if (fMask < fThreshold)
                discard;
            
            // 엣지 글로우
            float fEdge = smoothstep(fThreshold, fThreshold + g_fDissolveEdgeWidth, fMask);
            Out.vColor.rgb += (1.f - fEdge) * g_vDissolveEdgeColor;
        }
        else
        {
            if (fNoise < fLifeTime)
                discard;
        
            // 엣지 글로우
            if (g_bUseEdgeGlow)
            {
                float fEdge = smoothstep(fLifeTime, fLifeTime + g_fGlowStrength, fNoise);
                Out.vColor.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
            }
        }
    }
    if (g_bEmissive)
        Out.vEmissive = Out.vColor * 2.f;
    else
        Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
   //Velocity 계산
    if (Out.vColor.a >= 0.3f)
    {
        float2 curUV = In.vClipPos.xy / In.vClipPos.w;
        curUV = curUV * float2(0.5f, -0.5f) + 0.5f;
    
        float2 prevUV = In.vPrevClipPos.xy / In.vPrevClipPos.w;
        prevUV = prevUV * float2(0.5f, -0.5f) + 0.5f;
    
        Out.vVelocity = float4(curUV - prevUV, 0.f, 1.f);
    }
    else
        Out.vVelocity = float4(0.f, 0.f, 0.f, 1.f);
    
    // Soft Particle
    float2 vScreenUV = In.vProjPos.xy / In.vProjPos.w;
    vScreenUV = vScreenUV * 0.5f + 0.5f;
    vScreenUV.y = 1.f - vScreenUV.y;
    
    float fSceneDepth = g_DepthTexture.Sample(PointSampler, vScreenUV).y;
    if (fSceneDepth < 0.001f)
        return Out;
    
    float fMeshDepth = In.vProjPos.w;
    float fSoftFade = saturate((fSceneDepth - fMeshDepth) / 2.5f);
    Out.vColor.a *= fSoftFade;
    
    return Out;
}

PS_OUT PS_SCENE_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    float fLifeTime = saturate(g_fTimeElapsed / g_fLifeTime);   
    
    // 화면 텍스처의 UV 계산 (투영 좌표 => 0~1범위)
    float2 vScreenUV;
    vScreenUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vScreenUV.y = -In.vProjPos.y / In.vProjPos.w * 0.5f + 0.5f;
    
    // 마스크 텍스처 적용
    float2 vMaskUV = In.vTexcoord * g_vMaskUVScale;

    if (g_iMaskFrameCount > 1)
    {
        uint iMaskFrame = (uint) (fLifeTime * (float) g_iMaskFrameCount);
        iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
        iMaskFrame += g_iMaskStartFrame;

        float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
        float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);

        vMaskUV = In.vTexcoord * g_vMaskUVScale * vMaskFrameSize + vMaskOffset;
    }

    vMaskUV += g_vMaskUVStartOffset + g_vMaskUVScroll * g_fLifeTime;
    
    float fMask;
    if (g_iMaskSampler == 0)
        fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
    else if (g_iMaskSampler == 1)
        fMask = g_MaskTexture.Sample(BorderSampler, vMaskUV).r;

    fMask = saturate(fMask * g_fMaskIntensity);
    
    float2 vNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord).rg;
    float2 vOffset = (vNoise - 0.5f) * 2.f * g_fDistortionStrength * fMask;
    
    // 왜곡된 UV로 최종 장면 샘플링
    float4 vFinalScene = g_SceneTexture.Sample(LinearClampSampler, vScreenUV + vOffset);
    
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fLifeTime / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fLifeTime) / g_vFadeInOut.y) : 1.f;

    Out.vColor = vFinalScene;
    Out.vColor.a = fMask * fFadeIn * fFadeOut;
    
    if (g_bEmissive)
        Out.vEmissive = Out.vColor * 2.f;
    else
        Out.vEmissive = float4(0.f, 0.f, 0.f, 0.f);
    
    //Velocity 계산
    if (Out.vColor.a >= 0.3f)
    {
        float2 curUV = In.vClipPos.xy / In.vClipPos.w;
        curUV = curUV * float2(0.5f, -0.5f) + 0.5f;
    
        float2 prevUV = In.vPrevClipPos.xy / In.vPrevClipPos.w;
        prevUV = prevUV * float2(0.5f, -0.5f) + 0.5f;
    
        Out.vVelocity = float4(curUV - prevUV, 0.f, 1.f);
    }
    else
        Out.vVelocity = float4(0.f, 0.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out;
                                                                                                                           
    // 노이즈 UV 왜곡                                                                                                    
    float2 vNoiseUV = In.vTexcoord + float2(0.f, -g_fTimeElapsed * 0.3f);
    float2 vDistort = g_NoiseTexture.Sample(LinearSampler, vNoiseUV).rg;
    vDistort = (vDistort - 0.5f) * 2.f * g_fDistortionStrength;
    
    // 디퓨즈 텍스처 (왜곡 + 스크롤 적용)
    float2 vDiffuseUV = In.vTexcoord + g_vDiffuseUVScroll * g_fTimeElapsed + vDistort;
    float4 vColor = g_DiffuseTexture.Sample(LinearSampler, vDiffuseUV);
    
    if (vColor.a <= 0.2f)
        discard;
    
    // FadeIn/Out                                                                                                        
    float fLifeTime = saturate(g_fTimeElapsed / g_fLifeTime);
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fLifeTime / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fLifeTime) / g_vFadeInOut.y) : 1.f;
    vColor.a *= fFadeIn * fFadeOut;
    
    float fLuminance = dot(vColor.rgb, float3(0.299, 0.587, 0.114));
    float fHotFactor = pow(saturate(fLuminance), g_fHotColor);
    Out.vColor = vColor * g_vColor;
    Out.vColor.rgb = lerp(Out.vColor.rgb, g_vHotColor.rgb, fHotFactor);
    
      // 마스크 (필요 시)                                                                                                  
    if (g_bUseMask)
    {
        float2 vMaskUV = In.vTexcoord * g_vMaskUVScale + g_vMaskUVStartOffset + g_vMaskUVScroll * g_fTimeElapsed;
        float fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
        fMask = saturate(fMask * g_fMaskIntensity);
        Out.vColor.a *= fMask;
    }   
    Out.vEmissive = Out.vColor * 2.f;
    
    //Velocity 계산
    if(Out.vColor.a >= 0.3f)
    {
        float2 curUV = In.vClipPos.xy / In.vClipPos.w;
        curUV = curUV * float2(0.5f, -0.5f) + 0.5f;
    
        float2 prevUV = In.vPrevClipPos.xy / In.vPrevClipPos.w;
        prevUV = prevUV * float2(0.5f, -0.5f) + 0.5f;
    
        Out.vVelocity = float4(curUV - prevUV, 0.f, 1.f);
    }
    else
        Out.vVelocity = float4(0.f, 0.f, 0.f, 1.f);
    
    return Out;
}