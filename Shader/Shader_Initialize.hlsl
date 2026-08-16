#include "Shader_Buffer.hlsli"
#include "Sampler_Define.hlsli"

// 이 코드는 Google Gemini를 통해 작성되었습니다
// 순수 이니셜라이즈, 상수버퍼 변수 해시 초기화용으로 모든 변수 넣어뒀음
// 왜냐하면 쉐이더는 최종적으로 영향을 끼치는 return값에 변수가 영향을 주지 않는다고 판단하면 알아서 빼버리는
// 쓸데없이 개쳐똑똑한 놈이라서 이렇게 해야 전부 초기화가 가능
// 이거 언제 손으로 다 치고있어요!!!!!!!!!!!!!!!!!!!

// 정점 쉐이더
struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vDebug : TEXCOORD0; // 최적화 방지용 데이터 전달
};

// 정점 쉐이더
VS_OUT VS_MAIN(uint vID : SV_VertexID)
{
    VS_OUT Out;
    
    // -----------------------------------------------------
    // 최적화 방지용 누적 (VS 단계 버퍼들)
    // -----------------------------------------------------
    float4 vAccumulator = float4(0.f, 0.f, 0.f, 0.f);

    // [b0] Camera
    vAccumulator += g_vCamPosition;
    vAccumulator += g_ViewMatrix[0];
    vAccumulator += g_ProjMatrix[0];
    vAccumulator += g_InverseViewMatrix[0];
    vAccumulator += g_InverseProjMatrix[0];

    // [b1] Object
    vAccumulator += g_vColor;
    vAccumulator += g_WorldMatrix[0];
    vAccumulator += g_fProgress;
    vAccumulator += (float) g_ObjectID;

    // [b4] Bones
    vAccumulator += g_BonesMatrices[0][0];

    // [b5] MorphBuffer
    //vAccumulator += g_SlotEmpty;
    
    // [b8] Decalbuffer
    vAccumulator += g_InvDecalWorldMatrix[0];

    // -----------------------------------------------------
    // 출력 설정
    // -----------------------------------------------------
    Out.vPosition = float4(0.f, 0.f, 0.f, 1.f);
    Out.vDebug = vAccumulator; // 누적값을 PS로 넘김

    return Out;
}

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

// 픽셀 쉐이더
PS_OUT PS_MAIN(VS_OUT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // VS에서 넘어온 값을 이어받음
    float4 vAccumulator = In.vDebug;

    // -----------------------------------------------------
    // 최적화 방지용 누적 (PS 단계 버퍼들)
    // -----------------------------------------------------

    // [b2] Lights (빠짐없이 추가)
    vAccumulator += g_fLightRange;
    vAccumulator += g_vLightDirection;
    vAccumulator += g_vLightPosition;
    vAccumulator += g_vLightDiffuse;
    vAccumulator += g_vLightAmbient;
    vAccumulator += g_vLightSpecular;
    vAccumulator += g_LightViewMatrix[0];
    vAccumulator += g_LightProjMatrix[0];

    // [b3] Player
    vAccumulator.x += g_vPlayerInnerColor[0];
    
    // [b9] EffectBuffer
    vAccumulator.xy += g_vScaleUV;
    vAccumulator.z += g_fTransparency;
    vAccumulator.w += g_fAlpha;
    
    vAccumulator.x += (float) g_bIsBillboard;
    vAccumulator.y += (float) g_iFrameCount;
    vAccumulator.z += (float) g_iCountX;
    vAccumulator.w += (float) g_iCountY;
    vAccumulator.x += (float) g_iTransparentIndex;

    // [b10] UIBuffer
    vAccumulator += g_UIColor;
    
    vAccumulator.xy += g_UVScale;
    vAccumulator.zw += g_UVOffSet;
    
    vAccumulator.xy += g_UVScroll;
    vAccumulator.z += g_AlphaOffSet;
    vAccumulator.w += g_Reserved;

    vAccumulator.xy += g_TextureSize;
    vAccumulator.z += g_OutLineThickness;
    vAccumulator.w += g_UIPadding;
    
    vAccumulator += g_OutLineColor;

    //[b7] Empty
    vAccumulator.x += g_emptySlot;
    
    //[b6] AnimBuffer
    vAccumulator += asdfasdf;
    
    //b12] emptybuffer
    vAccumulator.x += g_ThisBufferIsEmpty;
    
    // -----------------------------------------------------
    // 최종 출력
    // -----------------------------------------------------
    Out.vColor = vAccumulator;
    
    return Out;
}