#pragma pack_matrix(row_major)
// Shader_Buffer.hlsli

cbuffer Camera : register(b0)
{
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
    float4x4 g_PrevViewMatrix;
    float4x4 g_PrevProjMatrix;
    float4x4 g_InverseViewMatrix;
    float4x4 g_InverseProjMatrix;
    float4 g_vCamPosition;
    
    //SSAO
    float4 g_Samples[32];
    float2 g_NoiseScale;
    float g_SSAORadius;
    float g_SSAOBias;
    float4x4 g_CamViewMatrix;
    float4x4 g_CamProjMatrix;
};

cbuffer Object : register(b1)
{
    float4x4 g_WorldMatrix;
    float4x4 g_PrevWorldMatrix;
    float4 g_vColor;
    float g_fProgress;
    uint g_ObjectID;
    uint g_bitFlag;
    float g_fTime;
    uint g_fRimOn;
    float2 g_vEmissionUVPos;
};

cbuffer Lights : register(b2)
{
    float g_fLightRange;
    float3 Light_Padding;
    float4 g_vLightDirection;
    float4 g_vLightPosition;
    float4 g_vLightDiffuse;
    float4 g_vLightAmbient;
    float4 g_vLightSpecular;
    float4x4 g_LightViewMatrix;
    float4x4 g_LightProjMatrix;  
    
    float4 lights_vDiffuse[64];
    float4 lights_vAmbient[64];
    float4 lights_vSpecular[64];
    float4 lights_vPosition[64];
    float4 lights_vRangeAndType[64];
    
    uint lightCount;
    float g_fSpotInnerCone;
    float g_fSpotOuterCone;
    
    float PaddingLight;
}

cbuffer Player : register(b3) //플레이어 전용으로 사용(많이 추가될 예정)
{
    //바디
    float4 g_vPlayerInnerColor[6]; //옷 전용 컬러
    float3 paddingBody;
    float g_fPlayerColorStrength; //강도(옷에서 사용중 ) 
    
    //헤어
    float4 g_vPlayerHairColor; //플레이어 머리색
    //Hair_json에서 값은 참조
    float g_fPlayerHighlight1Shift; //첫번째 하이라이트의 위치를 이동 , 머리색과 비슷한 색의 광택
    float g_fPlayerHighlight1Strength; //첫번째 하이라이트의 밝기(강도) 높을수록 머리카락이 반짝 , 기름짐
    float g_fPlayerHighlight2Shift; //두번째 하이라이트의 위치를 이동 / 1번과 살짝 어긋나야 자연스러운 값
    float g_fPlayerHighlight2Strength; //두번째 하이라이트의 밝기(강도) 높을수록 머리카락이 반짝 , 기름짐
    
    float g_fPlayerHighlightPower; //광택의 날카로움(모임 정도)
    float g_fPlayerMetallic; //낮게 설정 / 높이면 인조가발 느낌
    float g_fPlayerRoughnessMin; //최소 거칠기 / 빛이 너무 완벽하게 반사되는것을 막는 값
    float HairPadding33; //플레이어헤어메쉬번호

    //눈
    float4 g_vPlayerEyeBaseColor; //홍채의 기본(바깥쪽) 추측 (원작 커스터마이징 처럼 좌우 눈으로 하려면 Eye_R , Eye_L과 같이 구분해야함)
    float4 g_vPlayerEyeAppendColor; //홍채의 중심부 추측  

    float g_fPlayerEyeScale; //홍채크기
    float g_fPlayerEyeIndividualScaleOffset; //텍스처마다 홍채 크기/정렬 보정같음
    float g_fPlayerEyeOffsetU;  //눈동자 시선 제어
    float g_fPlayerEyeOffsetV; //눈동자 시선 제어
    
    float g_fPlayerEyeHighlightStrength; //하이라이트(반사광)강도
    
    float g_fPlayerTatooOnOff; //타투 ON/OFF
    float g_fPlayerTattoAlpha; // 타투 알파값
    float g_fPlayerTattoScale; //타투 크기
    float g_fPlayerTattoOffsetU; //타투 X위치
    float g_fPlayerTattoOffsetV; //타투 y위치
    float2 PaddingEye;
    float4 g_vPlayerTattoColor; //타투색깔

    
    float4 g_vPlayerBrowColor; //눈썹 색깔
    float4 g_vPlayerEyeLineColor; //아이라인 색깔
    float4 g_vPlayerLipColor; //립색깔
    
    float2  g_vPlayerBrowUVOffset; //json값 읽어오기
    float2  g_fPlayerBrowUVScale; //json값 읽어오기
    
    int     g_iPlayerUseSecondUV; //2번째 UV사용할것인지 여부
    float   g_fPlayerBrowUVRotation; //눈썹 회전
    float   g_fPlayerBrowXGap; //눈썹 좌우 간격
    float g_fEyeLineAlpha;
    float3 padding33;
    float g_fLipAlpha;
    //스킨,헤어,눈썹 등등 색깔 추가해야함.

}

// 무기 업데이트 해야돼서 필요......한가..?
cbuffer Bones : register(b4)
{
    float4x4 g_BonesMatrices[512];
}

cbuffer ShaderBuffer : register(b5)
{
     //칼라그레이딩용
    float3 g_vShadowTintColor; //색
    float g_fShadowTintWeight; //가중치

    float3 g_vMidtoneTintColor; //색
    float g_fMidtoneTintWeight; //가중치

    float3 g_vHighlightTintColor; //색
    float g_fHighlightTintWeight; //가중치
    
    float g_fShadowRange; //범위(0~Range)
    float g_fHighLightRange; //범위(high~1)
    float g_fEnableColorGrading; //On/OFF
    float PaddingColorGrading;
    
    //뎁스포그
    float3 g_FogColor; //색깔
    float g_bFogEnable; //ON/OFF
    
    float g_fFogStartDist; //포그 시작거리
    float g_fFogEndDist; //포그 적용거리
    float g_fFogDensity; //포그 빽빽함
    float g_fogType; //포그타입 0.5로 나뉨
    
    float g_fFogBaseHeight; //포그 가장 짙은 높이
    float g_fFogFadeHeight; //포그 사라질 위치
    float g_fHeightFogIntensity; //높이 포그 강도
    float g_fFogIntensity; //전체 포그 강도
    
    //PBR
    //조명
    float g_fLightMultiplier; //전체 조명 세기 
    float g_fAmbientStrength; //앰비언트 강도
    float2 g_vToonShadowRange; //툰그림자 경계범위
    
    float2 g_vToonBrightnessRange; //그림자 밝은면 최소/최대 밝기
    float2 g_vAmbientFloorMin; //최소 앰비언트 보장
    
    // SSS / Skin
    float3 g_vSSSColor; //SSS색상
    float g_fBackFillStrength; //뒷면 보정강도(이건조명용)
    
    float3 g_vSkinTint; //피부톤
    float g_fSSSPower; //SSS투과정도
    
    float g_fSSSIntensity; //강도
    float g_fSkinRoughnessMin; //피부최소 Rough

    // Rim
    float g_fRimPower; //림폭(낮으면 넓어짐)
    float g_fRimIntensity; //림 강도
    float2 g_vRimMaskRange; //림 마스크범위

    // Specular
    float2 g_vSpecBoostRange; //강한건 강하게 약한건 약하게
    float g_fSpecularIntensity; //전체 스펙 세기
    float g_fMinDiffuse; //디퓨즈 보장
    
    float2 g_vSpecBosstMapRange;
    float g_fSpecularMapIntensity; //맵 스펙 세기
    float g_fUnderMapZeroDirSpecular; //지하 directional스페큘러 없애기
    
    //PBR보정
    float g_fKValue; //함수 엄격도?
    float g_fMtrlRoughnessMin; //물체 최소 Rough
    
    float2 g_vBackFaceShaowRange; //그림자 뒷면 범위
    float g_fSpecSoftClamp; //옷에 반짝거리는거 제거위해서
    float g_fShadowBright; //그림자 최소 밝기
    
    float3 Padding123;
    float g_fShadowBackBright; //그림자 뒷면 밝기
    
    //진짜 갓레이 
    float g_fGodRayDensity; // 빛줄기 길이 (높을수록 길어짐)
    float g_fGodRayDecay; // 거리 감쇠 (1에 가까울수록 멀리까지)
    float g_fGodRayWeight; // 샘플당 기여도
    float g_fGodRayExposure; // 최종 밝기
    float4 g_vGodRayColor; // 갓레이 색
    float4 g_vGodRayPosition; //갓레이 위치
    
    //GodRay메쉬조정용
    float g_fGodRayMeshScrollSpeed; // UV 스크롤 속도
    float g_fGodRayMeshSwayFreq; // 좌우 흔들림 빈도
    float g_fGodRayMeshSwayPhase; // 높이별 위상 차이
    float g_fGodRayMeshSwayAmp; // 좌우 흔들림 폭
    
    float3 g_vGodRayMeshColor; // 빛 색상
    float g_fGodRayMeshContrast; // 노이즈 대비
    
    float g_fGodRayMeshIntensity; // 전체 밝기
    float g_fGodRayMeshEdgeFade; // 가장자리 페이드 폭
    float g_fSoftParticleRange;
    float g_fGodRayMeshAngleFadePower;
    
    float g_fGodRayMeshFadeNear;
    float g_fGodRayMeshFadeFar;
    
    //뱀부, 동굴젖은정도
    float g_fBambooEmission;
    float g_fUnderBambooEmission;
    float g_fCaveMapWet;
    
    //CamMotionBlur
    float g_fCamMotionBlurIntensity;
    float g_fCamMotionBlurMaxVelocity;
    float g_fCamMotionBlurSamples;
    
    
    //몬스터 스페큘러
    float g_fMonsterSpecularIntensity;
    float2 g_vSpecMonsterBoostRange;
    float padding;
}

cbuffer EmptyBuffer1 : register(b6)
{
    float4 asdfasdf;
}

cbuffer EmptyBuffer2 : register(b7)
{
    float4 g_emptySlot;
}

cbuffer DECAL : register(b8)
{
    float4x4 g_InvDecalWorldMatrix;
    
    float g_fDecalAlpha;
    uint g_iMaskIndex;
    float2 g_DecalPadding;
}

cbuffer EffectBuffer : register(b9)
{
    float4x4 g_EffectWorldMatrix;
    
    float2 g_vScaleUV;
    float g_fTransparency;
    float g_fAlpha;
    
    float g_fDistortionStrength;
    uint g_bIsBillboard;
    uint g_iFrameCount;
    uint g_iCountX;
    
    uint g_iCountY;
    uint g_iTransparentIndex;
    uint g_bUseMask;
    uint g_bRadialRotation;
    
    float3 g_vEffectPivot;
    uint g_iMaskStartFrame;

    uint g_iMaskFrameCount;
    uint g_iMaskCountX;
    uint g_iMaskCountY;
    float g_fParticleScale;
    
    float2 g_vMaskUVScroll;
    uint g_bUseNoise;
    uint g_bVerticalFade;
    
    uint g_bUseTexture;
    uint g_bUseAlphaMask;
    uint g_bUseEdgeGlow;
    float g_fGlowStrength;
    
    float g_fLifeTime;
    float g_fTimeElapsed;
    float2 g_vDiffuseUVScroll;
    
    float2 g_vMaskUVScale;
    float2 g_vFadeInOut;
    
    float g_fMaskIntensity;
    float2 g_vMaskUVStartOffset;
    uint g_iMaskSampler;
    
    uint g_bDirectionalDissolve;
    uint g_bDissolveFlipY;
    float g_fNoiseWeight;
    float g_fDissolveEdgeWidth;

    uint g_bVerticalShrink;
    float3 g_vDissolveEdgeColor;
    
    float g_fDisplaceStrength;
    float3 g_vHotColor;
    
    float g_fHotColor;
    int g_iDirectionalScale;
    uint g_bEmissive;    
    float2 g_vEffectOffset;
    float3 g_vEffectPadding;
};

cbuffer UIBuffer : register(b10)
{
    float4 g_UIColor;
    
    float2 g_UVScale;
    float2 g_UVOffSet;
    
    float2 g_UVScroll;
    float g_AlphaOffSet;
    float g_Reserved;

    float2 g_TextureSize;
    float g_OutLineThickness;
    float g_UIProgress;
    
    float4 g_OutLineColor;
    
    float g_ScrollTime;
    float g_fClipX;
    float2 g_UIPadding;
    
    
    float g_fUIValue; //기냥 던질값. 아무거나사용가능
    float2 g_fClip;
    float g_fUIIntensity = 1.f;
    
    
}

cbuffer ShadowBuffer : register(b11)
{
    float4x4 g_ShadowCascade_ViewProjMatrix;
    float4x4 g_Shadow_ViewProjMatrix;
    float4x4 g_ShadowCascade_MixMaxtrix[4];
    float4 g_ShadowSplit;
    uint g_ShadowCascadeNum;
    float3 ShadowPadding;
    float4 g_HDROption;
    float4 g_SSAOOption;
    float4 g_ShadowDir;
}

cbuffer EmptyBuffer3 : register(b12)
{
    uint4 g_ThisBufferIsEmpty;
}

cbuffer MinimapBuffer : register(b13)
{
    //For Minimap
    float2 g_vTargetUV;     //객체의 월드에서 UV로 변환한 좌표
    float g_fRotation;      //객체회전값
    float g_fIconScale = 0.05f;     //미니맵 대비 아이콘의 크기
    
    float2 g_MinimapOffSet;
    float g_fZoom = 1.0f;   //지형 확대 배율
    float g_Dummy1;
    
    float2 g_vCenterUV;
    float g_fCameraAngle;
    float g_Dummy2;
    
    float2 g_vPlayerUV;
    float2 g_Dummy3;
    
        
    int g_isClipping;
    float3 g_Dummy4;
    
    //For RTV-BLUR
    float2 g_vBlurTargetSize;
    float2 g_Dummy5;

}

float3 DecodeNormalBC5(Texture2D<float4> normalTex, SamplerState samp, float2 uv)
{
    float2 rg = normalTex.Sample(samp, uv).rg * 2.f - 1.f;
    float z = sqrt(1.f - saturate(dot(rg, rg)));
    return float3(rg, z);
}

struct IdMasks
{
    float L1;
    float L2;
    float L3;
    float L4;
    float L5;
    float L6;
    float fAny;
};

IdMasks DecodeIdMap6(float3 Id) //ID의6개색 풀기
{
    IdMasks tIdMasks;
    //Id.r이 0.5보다 크거나 같으면 1 , 작으면0
    float R = step(0.5f, Id.r);
    float G = step(0.5f, Id.g);
    float B = step(0.5f, Id.b);
    
    float NewR = 1.0f - R; //Red 아닌 영역 추출
    float NewG = 1.0f - G; //Green 아닌 영역 추출
    float NewB = 1.0f - B; //Blue 아닌 영역 추출
    
    float MaskRed = R * NewG * NewB; //빨강만있고 나머지 없음
    float MaskGreen = NewR * G * NewB; //초록만있고 나머지 없음
    float MaskBlue = NewR * NewG * B; //파랑만있고 나머지 없음 
    float MaskCyan = NewR * G * B; //청녹색만 있고 나머지 없음
    float MaskMagenta = R * NewG * B; //마젠타 만있고 나머지 없음 
    float MaskYellow = R * G * NewB; //노랑 만있고 나머지 없음 

    tIdMasks.L1 = MaskRed;
    tIdMasks.L2 = MaskGreen;
    tIdMasks.L3 = MaskBlue;
    tIdMasks.L4 = MaskCyan;
    tIdMasks.L5 = MaskMagenta;
    tIdMasks.L6 = MaskYellow;
    tIdMasks.fAny = saturate(MaskRed + MaskGreen + MaskBlue + MaskCyan + MaskMagenta + MaskYellow); //개별 부위 마스크를 모두 합쳐서 옷 전체 중 어디가 염색 대상인지 결정
    return tIdMasks;
}

static const float4 DefaultDiffuse      = float4(0.f, 0.f, 0.f, 1.f);       // alpha 0 -> Combined에서 discard (스카이박스 통과)
static const float4 DefaultNormal       = float4(0.5f, 0.5f, 1.f, 1.f);     // xyz: 탄젠트 기본 노멀(0,0,1) 인코딩, w: SpecularStrength = 0
static const float4 DefaultDepth        = float4(0.f, 1.f, 0.f, 1.f);       // x: NDC z=1(최원거리), y: ViewZ=0(배경판별용), z: AmbientStrength=0, w: Shininess=0
static const float4 DefaultPick         = float4(0.f, 0.f, 0.f, 1.f);       // 피킹 대상 아님
static const float4 DefaultPBR          = float4(0.f, 0.5f, 1.f, 1.f);      // r: Metalic/SSS=0, g: Roughness=0.7(무난), b: AO=0
static const float4 DefaultSpecular     = float4(0.04f, 0.04f, 0.04f, 1.f); // 비금속 기본 반사율 F0
static const float4 DefaultRoughness    = float4(0.f, 0.f, 0.f, 1.f);       // r < 0.5 -> SSS 아닌 일반 경로
static const float4 DefaultEmissive     = float4(0.f, 0.f, 0.f, 1.f);       // 발광 없음

#define FLAGDIFFUSE (1 << 1)
#define FLAGSPECULAR  (1 << 2)
#define FLAGEMISSIVE (1 << 4)
#define FLAGNORMAL (1 << 6)
#define FLAGSHININESS (1 << 7)
#define FLAGOPACITY (1 << 8)
#define FLAGMETALNESS (1 << 15)
// 일단 모프(표정)은 서비스 종료다...
//cbuffer MorphBuffer : register(b5)
//{
//    float4 g_MorphWeightsPacked[8]; // 32개의 weight를 8개의 float4로
//    uint4 g_MorphIndicesPacked[8]; // 32개의 index를 8개의 uint4로
//    uint g_NumActiveMorphs;
//    uint g_NumVertices;
//    uint2 _padding; // 16바이트 정렬용
//}