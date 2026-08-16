#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// Texture2D + SamplerState 분리해서 선언
Texture2D<float4> g_Texture : register(t0);
Texture2D<float4> g_MaskTexture : register(t1);
Texture2D<float4> g_Texture_Scene : register(t2);
Texture2D<float4> g_Texture_Noise : register(t3);
Texture2D<float4> g_Texture_Alpha : register(t4);
Texture2D<float4> g_Texture_Depth : register(t30);

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
    float2 vRotation : TEXCOORD6;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vClipPos : TEXCOORD3;
    float4 vPrevClipPos : TEXCOORD4;
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
        
        if (In.vLifeTime.x < 0.f)
        {
            scaleX = 0.f;
            scaleY = 0.f;
        }
        
        // 수명 비율에 따라서 크기를 보간해준다.
        float fLifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
        float fTextureScale = lerp(1.f, g_fParticleScale, fLifeRatio);
        
        if (g_bVerticalFade)
        {
            // 가로만 줄어들기
            scaleX *= fTextureScale;
            
            // 세로도 동시에 줄어드는 옵션
            if (g_bVerticalShrink)
                scaleY *= (2.f / max(fTextureScale, 0.01f));
        }
        else
        {
            scaleX *= fTextureScale;
            scaleY *= fTextureScale;
        }
        
        float scaleAfterX = (In.vPosition.x - g_vEffectOffset.x) * scaleX;
        float scaleAfterY;
        if (g_iDirectionalScale != 0)
        {
            float fPivot = (float) g_iDirectionalScale * 0.5f;
            scaleAfterY = scaleY * (In.vPosition.y - g_vEffectOffset.y - fPivot) * fTextureScale + fPivot;
        }
        else
        {
            scaleAfterY = (In.vPosition.y - g_vEffectOffset.y) * scaleY;
        }
        
        // 회전 관련
        float fPivotAngle = In.vRotation.y * 3.14f / 180.f;
        float fRadialAngle = 0.f;
        
        if (g_bRadialRotation == 1)
        {
            float3 vDir = In.vTranslation.xyz - g_vEffectPivot;
            float fProjX = dot(vDir, camRight);
            float fProjY = dot(vDir, camUp);
            fRadialAngle = atan2(fProjY, fProjX);
        }
        
        float fAngle = fRadialAngle + fPivotAngle;
        
        float fCos = cos(fAngle);
        float fSin = sin(fAngle);
        
        float fRotationX = scaleAfterX * fCos - scaleAfterY * fSin;
        float fRotationY = scaleAfterX * fSin + scaleAfterY * fCos;

        // 빌보드 정점 위치 계산
        float3 localPos = In.vTranslation.xyz
                      + camRight * fRotationX
                      + camUp * fRotationY;

        float3 vEffectPos = float3(g_EffectWorldMatrix._41, g_EffectWorldMatrix._42, g_EffectWorldMatrix._43);
        float4 worldPos = float4(localPos + vEffectPos, 1.f);
        
        float3 vPrevEffectPos = float3(g_PrevWorldMatrix._41, g_PrevWorldMatrix._42, g_PrevWorldMatrix._43);
        float4 PrevworldPos = float4(localPos + vPrevEffectPos, 1.f);
        // float4 worldPos = mul(float4(localPos, 1.f), g_EffectWorldMatrix);
        
        float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
        Out.vPosition = mul(worldPos, matVP);
        
        Out.vClipPos = mul(worldPos, matVP);
        Out.vPrevClipPos = mul(PrevworldPos, matVP);
    }
    else
    {
        float4 vRight = In.vRight;
        float4 vUp = In.vUp;
        float4 vLook = In.vLook;
        
        // 딜레이 중 안보이게
        if (In.vLifeTime.x < 0.f)
        {
            vRight = float4(0.f, 0.f, 0.f, 0.f);
            vUp = float4(0.f, 0.f, 0.f, 0.f);
        }
        
        // 회전 적용
        float fPivotAngle = In.vRotation.y * 3.14f / 180.f;
        float fRadialAngle = 0.f;
        
        if (g_bRadialRotation == 1)
        {
            float3 vDir = In.vTranslation.xyz - g_vEffectPivot;
            float3 vRadialRight = normalize(vRight.xyz);
            float3 vRadialUp = normalize(vUp.xyz);
            float fProjX = dot(vDir, vRadialRight);
            float fProjY = dot(vDir, vRadialUp);
            fRadialAngle = atan2(fProjY, fProjX);
        }
        
        float fAngle = fRadialAngle + fPivotAngle;
        
        float fCos = cos(fAngle);
        float fSin = sin(fAngle);
        
        float4 vRotationRight = vRight * fCos - vUp * fSin;
        float4 vRotationUp = -vRight * fSin + vUp * fCos;
        
        // 수명 비율에 따라서 크기를 보간해준다.
        float fLifeRatio = saturate(In.vLifeTime.x / In.vLifeTime.y);
        float fTextureScale = lerp(1.f, g_fParticleScale, fLifeRatio);
        
        if (g_iDirectionalScale != 0)
        {
            vRotationRight *= fTextureScale;
        }
        else if (g_bVerticalFade)
        {
            // 가로만 줄어들기
            vRotationRight *= fTextureScale;
            
            // 세로도 동시에 줄어드는 옵션
            if (g_bVerticalShrink)
                vRotationUp *= (2.f / max(fTextureScale, 0.01f));
        }
        else
        {
            vRotationRight *= fTextureScale;
            vRotationUp *= fTextureScale;
        }
        
        // g_EffectWorldMatrix에서 회전만 추출
        float3x3 effectRotation = (float3x3)g_EffectWorldMatrix;
        
        // 파티클 축을 이펙트 회전으로 돌리기
        float4 vRotateRight = float4(mul(vRotationRight.xyz, effectRotation), 0.f);
        float4 vRotateUp = float4(mul(vRotationUp.xyz, effectRotation), 0.f);
        float4 vRotateLook = float4(mul(In.vLook.xyz, effectRotation), 0.f);
        
        float3x3 PreveffectRotation = (float3x3) g_PrevWorldMatrix;
        float4 vPrevRotateRight = float4(mul(vRotationRight.xyz, PreveffectRotation), 0.f);
        float4 vPrevRotateUp = float4(mul(vRotationUp.xyz, PreveffectRotation), 0.f);
        float4 vPrevRotateLook = float4(mul(In.vLook.xyz, PreveffectRotation), 0.f);
        
        float4x4 matTransformationMatrix = float4x4(vRotateRight, vRotateUp, vRotateLook, In.vTranslation);
        float4x4 matPrevTransformationMatrix = float4x4(vPrevRotateRight, vPrevRotateUp, vPrevRotateLook, In.vTranslation);
        float3 vLocalPos = In.vPosition;
        if (g_iDirectionalScale != 0)
        {
            float fPivot = (float) g_iDirectionalScale * 0.5f;
            vLocalPos.y = (vLocalPos.y - fPivot) * fTextureScale + fPivot;
        }
    
        float4 vPosition = mul(float4(vLocalPos, 1.f), matTransformationMatrix);
        float3 vEffectPos = float3(g_EffectWorldMatrix._41, g_EffectWorldMatrix._42, g_EffectWorldMatrix._43);
        float4 worldPos = float4(vPosition.xyz + vEffectPos, 1.f);
        
        float4 vPrevPosition = mul(float4(vLocalPos, 1.f), matPrevTransformationMatrix);
        float3 vPrevEffectPos = float3(g_PrevWorldMatrix._41, g_PrevWorldMatrix._42, g_PrevWorldMatrix._43);
        float4 PrevworldPos = float4(vPrevPosition.xyz + vPrevEffectPos, 1.f);
        
        float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
        Out.vPosition = mul(worldPos, matVP);
        Out.vClipPos = mul(worldPos, matVP);
        Out.vPrevClipPos = mul(PrevworldPos, matVP);
    }
    
    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vClipPos : TEXCOORD3;
    float4 vPrevClipPos : TEXCOORD4;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
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
        vColor = g_Texture.Sample(LinearSampler, vDiffuseUV);
        
        if (vColor.a <= 0.3f)
            discard;
    }
    else
    {
        vColor = float4(1.f, 1.f, 1.f, 1.f);
    }

    float fAlphaLifeTime = saturate(In.vLifeTime.x / In.vLifeTime.y);       // 0 => 1
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fAlphaLifeTime / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fAlphaLifeTime) / g_vFadeInOut.y) : 1.f;
    vColor.a *= fFadeIn * fFadeOut;

    Out.vColor = vColor * g_vColor;
    
    // 알파 마스크
    if (g_bUseAlphaMask)
    {
        float fAlphaMask = g_Texture_Alpha.Sample(LinearSampler, In.vTexcoord).r;
        
        Out.vColor.a *= fAlphaMask;
    }

    // 마스크 텍스처 적용
    if (g_bUseMask)
    {
        float2 vMaskUV = In.vTexcoord;
        
        if (g_iMaskFrameCount > 1)
        {
            float fProgress = saturate(In.vLifeTime.x / In.vLifeTime.y);
            uint iMaskFrame = (uint) (fProgress * (float) g_iMaskFrameCount);
            iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
            iMaskFrame += g_iMaskStartFrame;
            
            float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
            float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);
            
            vMaskUV = In.vTexcoord * vMaskFrameSize + vMaskOffset;
        }
        
        vMaskUV += g_vMaskUVScroll * In.vLifeTime.x;
        
        float fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
        fMask = saturate(fMask * g_fMaskIntensity);        
        Out.vColor.a *= fMask;
    }
    
    // Noise 텍스처
    if (g_bUseNoise)
    {
        float fNoise = g_Texture_Noise.Sample(LinearSampler, In.vTexcoord).r;
        float fDissolveThreshold = saturate(In.vLifeTime.x / In.vLifeTime.y);       // 0 ~ 1
        
        if (fNoise < fDissolveThreshold)
            discard;
        
        // 엣지 글로우
        if (g_bUseEdgeGlow)
        {
            float fEdge = smoothstep(fDissolveThreshold, fDissolveThreshold + g_fGlowStrength, fNoise);
            Out.vColor.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
        }
    }
    
    if (g_bEmissive)
        Out.vEmissive = Out.vColor;
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
    
    float fSceneDepth = g_Texture_Depth.Sample(PointSampler, vScreenUV).y;
    if (fSceneDepth < 0.001f)
        return Out;
    
    float fParticleDepth = In.vProjPos.w;
    float fSoftFade = saturate((fSceneDepth - fParticleDepth) / 2.5f);
    Out.vColor.a *= fSoftFade;
    
    return Out;
}

PS_OUT PS_SPRITE(PS_IN In)
{
    PS_OUT Out;
    
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
    
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fProgress / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fProgress) / g_vFadeInOut.y) : 1.f;
    
    vColor.rgba *= fTransparency * g_vColor;
    Out.vColor = vColor * g_fAlpha;
    Out.vColor.a *= fFadeIn * fFadeOut;
    
    // 마스크 텍스처 적용
    if (g_bUseMask)
    {
        float2 vMaskUV = In.vTexcoord;
        
        if (g_iMaskFrameCount > 1)
        {
            uint iMaskFrame = (uint) (fProgress * (float) g_iMaskFrameCount);
            iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
            iMaskFrame += g_iMaskStartFrame;
            
            float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
            float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);
            
            vMaskUV = In.vTexcoord * vMaskFrameSize + vMaskOffset;
        }
        
        vMaskUV += g_vMaskUVScroll * In.vLifeTime.x;
        
        float fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
        fMask = saturate(fMask * g_fMaskIntensity);
        Out.vColor.a *= fMask;
    }
    
        // Noise 텍스처
    if (g_bUseNoise)
    {
        float fNoise = g_Texture_Noise.Sample(LinearSampler, In.vTexcoord).r;
        float fDissolveThreshold = saturate(In.vLifeTime.x / In.vLifeTime.y); // 0 ~ 1
        
        if (fNoise < fDissolveThreshold)
            discard;
        
        // 엣지 글로우
        if (g_bUseEdgeGlow)
        {
            float fEdge = smoothstep(fDissolveThreshold, fDissolveThreshold + g_fGlowStrength, fNoise);
            Out.vColor.rgb += (1.f - fEdge) * float3(1.f, 0.5f, 0.1f); // 주황색
        }
    }
    
    if (g_bEmissive)
        Out.vEmissive = Out.vColor;
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
    
    float fSceneDepth = g_Texture_Depth.Sample(PointSampler, vScreenUV).y;
    if (fSceneDepth < 0.001f)
        return Out;
        
    float fParticleDepth = In.vProjPos.w;
    float fSoftFade = saturate((fSceneDepth - fParticleDepth) / 2.5f);
    Out.vColor.a *= fSoftFade;
    
    return Out;
}

PS_OUT PS_SCENE_DISTORTION(PS_IN In)
{
    PS_OUT Out;
    
    float fProgress = saturate(In.vLifeTime.x / In.vLifeTime.y);
    
    // 화면 텍스처의 UV 계산 (투영 좌표 => 0~1범위)
    float2 vScreenUV;
    vScreenUV.x = In.vProjPos.x / In.vProjPos.w * 0.5f + 0.5f;
    vScreenUV.y = -In.vProjPos.y / In.vProjPos.w * 0.5f + 0.5f;
    
    // 마스크 텍스처 적용
    float2 vMaskUV = In.vTexcoord;
    
    if (g_iMaskFrameCount > 1)
    {
        uint iMaskFrame = (uint) (fProgress * (float) g_iMaskFrameCount);
        iMaskFrame = min(iMaskFrame, g_iMaskFrameCount - 1);
        iMaskFrame += g_iMaskStartFrame;
           
        float2 vMaskFrameSize = float2(1.f / (float) g_iMaskCountX, 1.f / (float) g_iMaskCountY);
        float2 vMaskOffset = float2((float) (iMaskFrame % g_iMaskCountX) * vMaskFrameSize.x,
                                        (float) (iMaskFrame / g_iMaskCountX) * vMaskFrameSize.y);
            
        vMaskUV = In.vTexcoord * vMaskFrameSize + vMaskOffset;
    }
        
    vMaskUV += g_vMaskUVScroll * In.vLifeTime.x;
    
    float fMask = g_MaskTexture.Sample(LinearSampler, vMaskUV).r;
    fMask = saturate(fMask * g_fMaskIntensity);
    
    float2 vNoise = g_Texture_Noise.Sample(LinearSampler, In.vTexcoord).rg;
    float2 vOffset = (vNoise - 0.5f) * 2.f * g_fDistortionStrength * fMask;
    
    // 왜곡된 UV로 최종 장면 샘플링
    float4 vFinalScene = g_Texture_Scene.Sample(LinearClampSampler, vScreenUV + vOffset);
    
    float fFadeIn = (g_vFadeInOut.x > 0.f) ? saturate(fProgress / g_vFadeInOut.x) : 1.f;
    float fFadeOut = (g_vFadeInOut.y > 0.f) ? saturate((1.f - fProgress) / g_vFadeInOut.y) : 1.f;
    
    Out.vColor = vFinalScene;
    Out.vColor.a = fMask * fFadeIn * fFadeOut;
    
    if (g_bEmissive)
        Out.vEmissive = Out.vColor;
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