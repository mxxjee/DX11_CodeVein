#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

// ========== 저장할 때 사용하는 헤더 ==========
struct FileHeader
{
    _string version = {};      // 파일 포맷 버전
    _string toolType = {};     // 툴 타입 (MAP_TOOL, EFFECT_TOOL 등)
    _string createdAt = {};    // 생성 시간
    _string modifiedAt = {};   // 수정 시간

    json To_Json() const
    {
        return json{
            {"version", version},
            {"toolType", toolType},
            {"createdAt", createdAt},
            {"modifiedAt", modifiedAt}
        };
    }

    void From_Json(const json& jsonData)
    {
        version = jsonData.value("version", "1.0.0");
        toolType = jsonData.value("toolType", "UNKNOWN");
        createdAt = jsonData.value("createdAt", "");
        modifiedAt = jsonData.value("modifiedAt", "");
    }
};


// ========== ObjectKey 타입 정의 ==========
// 예: "Model/Wonder_Acute/Wonder_Acute"
using ObjectKey = _string;


// ========== 맵툴용 정보 ==========
struct MapObjectInfo
{
    ObjectKey       strObjectKey = "";            // 오브젝트 고유 키 (매칭용) - /Resources/ 이후 경로
    _wstring        wstrPrototypeName = L"";      // 프로토타입 이름 (Clone용)
    _wstring        wstrLayerName = L"";          // 배치할 레이어
    _float4x4       matWorld = {};                // 월드 행렬
    _uint           iObjectType = 0;              // 오브젝트 타입 (enum으로 관리 권장)

    ordered_json    jExtraData;                   // 기타 데이터 

    // === JSON 직렬화 ===
    ordered_json To_Json() const
    {
        ordered_json j;
        j["ObjectKey"] = strObjectKey;
        j["PrototypeName"] = wstringToString(wstrPrototypeName);
        j["LayerName"] = wstringToString(wstrLayerName);
        j["ObjectType"] = iObjectType;

        // 행렬은 16개 float 배열로 저장
        j["WorldMatrix"] = vector<_float>(
            reinterpret_cast<const _float*>(&matWorld),
            reinterpret_cast<const _float*>(&matWorld) + 16
        );

        if (!jExtraData.empty())
            j["ExtraData"] = jExtraData;

        return j;
    }

    void From_Json(const ordered_json& j)
    {
        strObjectKey = j.value("ObjectKey", "");
        wstrPrototypeName = stringToWstring(j.value("PrototypeName", ""));
        wstrLayerName = stringToWstring(j.value("LayerName", ""));
        iObjectType = j.value("ObjectType", 0u);

        if (j.contains("WorldMatrix"))
        {
            auto arr = j["WorldMatrix"].get<vector<_float>>();
            if (arr.size() == 16)
                memcpy(&matWorld, arr.data(), sizeof(_float4x4));
        }

        if (j.contains("ExtraData"))
            jExtraData = j["ExtraData"];

    }
};


// ========== 쉐이더툴용 정보 ==========
struct ShaderObjectInfo
{
    ObjectKey       strObjectKey = "";            // 오브젝트 고유 키 (매칭용)
    _uint           iObjectType = 0;              // 오브젝트 타입
    _wstring        wstrShaderPrototype = L"";    // 사용할 쉐이더 프로토타입
    vector<_int>   vecMeshPasses = {};           // 메쉬별 패스 번호

    ordered_json To_Json() const
    {
        ordered_json j;
        j["ObjectKey"] = strObjectKey;
        j["ObjectType"] = iObjectType;
        j["ShaderPrototype"] = wstringToString(wstrShaderPrototype);
        j["MeshPasses"] = vecMeshPasses;
        return j;
    }

    void From_Json(const ordered_json& j)
    {
        strObjectKey = j.value("ObjectKey", "");
        iObjectType = j.value("ObjectType", 0u);
        wstrShaderPrototype = stringToWstring(j.value("ShaderPrototype", ""));
        if (j.contains("MeshPasses"))
            vecMeshPasses = j["MeshPasses"].get<vector<_int>>();
    }
};


// ========== 이펙트툴용 정보 ==========
// SubEmitterInfo 데이터
struct SubEmitterInfo
{
    _uint       eType = 0;              // SUBEMMITER_TYPE (BIRTH = 0, DEATH = 1, COLLISION = 2)
    _wstring    wstrEffectName = {};
    _float      fEmitProbability = {};
    _bool       bInheritPosition = {};
    _bool       bInheritScale = {};

    ordered_json To_Json() const
    {
        ordered_json j;
        j["EmitterType"] = eType;
        j["EffectName"] = wstringToString(wstrEffectName);
        j["EmitProbability"] = fEmitProbability;
        j["InheritPosition"] = bInheritPosition;
        j["InheritScale"] = bInheritScale;

        return j;
    }

    void From_Json(const ordered_json& j)
    {
        eType = j.value("EmitterType", 0);
        wstrEffectName = stringToWstring(j.value("EffectName", ""));
        fEmitProbability = j.value("EmitProbability", 1.f);
        bInheritPosition = j.value("InheritPosition", true);
        bInheritScale = j.value("InheritScale", false);
    }
};

// 개별 이펙트의 데이터
struct ParticleEffectInfo
{
    _wstring    wstrEffectName = {};
    EFFECTTYPE  eEffectType = {};               // 0이면 BasicParticle, 1이면 SwordTrail, 2이면 MeshEffect
    _float      fStartDelay = {};

    // ParticleEffect 데이터
    _string     strTextureName = {};
    _string     strNoiseTextureName = {};
    _string     strMaskTextureName = {};
    _string     strAlphaMaskTextureName = {};
    _string     strShaderTag = {};
    _string     strComputeShaderTag = {};
    _float3     vEffectRotation = {};
    _bool       bUseEdgeGlow = {};
    _bool       bBackCulling = {};
    _bool       bEffectLoop = {};
    _bool       bEmissive = {};
    _int        iPassIndex = {};
    _uint       iMaskSampler = {};
    _float      fGlowStrength = {};
    _float      fEffectLifeTime = {};
    _float      fMaskIntensity = { 1.f };
    _float2     vDiffuseUVScroll = {};
    _float2     vFadeInOut = {};
    _float4     vColor = { 1.f, 1.f, 1.f, 1.f };
    _float      fHotColor = {};
    _float3     vHotColor = {};
    _float2     vEffectOffset = { 0.f, 0.f };

    // PARTICLE_DESC 데이터
    _bool       IsLoop = { false };             // 파티클이 반복되는 파티클인지
    _bool       IsBillboard = { true };         // 빌보드 여부
    _bool       IsVerticalFade = { false };     // 가로로 사라지는지?
    _bool       IsVerticalShrink = { false };   // 세로로 사라지는지?
    _bool       IsRadialRotation = { false };   // 피봇 바라보게 회전 여부
    _uint       iNumInstance = {};              // 인스턴스 개수(입자의 개수)
    _float2     vSize = {};                     // 입자의 크기
    _float2     vSpeed = {};                    // 입자의 속도(x ~ y 사이 값) 
    _float2     vLifeTime = {};                 // 입자의 생명주기
    _float2     vStartDelay = {};               // 입자별 시작 딜레이
    _float2     vRotation = {};                 // 회전 값 (x ~ y 사이 값)
    _float3     vCenter = {};                   // 입자의 중점
    _float3     vRange = {};                    // 입자가 퍼질 범위
    _float3     vPivot = {};                    // 입자의 피벗(파티클 방향을 잡아주는 역할)
    _float      fGravity = {};                  // 입자의 중력
    _float      fAlpha = { 1.f };               // 전체 알파값
    _uint       iFrameCount = { 1 };            // 스프라이트 프레임 개수
    _uint       iCountX = { 1 };                // 스프라이트 행 개수
    _uint       iCountY = { 1 };                // 스프라이트 열 개수
    _uint       iTransparentIndex = {};         // 스프라이트 투명 적용 시점 설정
    _uint       iMaskStartFrame = {};           // 마스크 스프라이트 시작 프레임
    _uint       iMaskFrameCount = { 1 };        // 마스크 스프라이트 프레임 개수
    _uint       iMaskCountX = { 1 };            // 마스크 스프라이트 행 개수
    _uint       iMaskCountY = { 1 };            // 마스크 스프라이트 열 개수
    _float2     vMaskUVScroll = {};             // 마스크 UV 스크롤 값
    _float      fParticleScale = {};            // 시간에 따른 최종 Scale 배율
    _int        iDirectionalScale = {};         // Scale 배율 방향성 적용
    _float      fShakeStrength = {};            // 무작위 움직임 강도
    _float      fConvergeMaxDist = {};          // CS_CONVERGE 용 변수 모이는 중앙 위치의 최대 거리 (구 형태)

    // Trail Effect 데이터 (iEffectType = 1일 경우 사용)
    _uint       iMaxPoints = { 64 };			// 최대 제어점 개수
    _float      fLifeTime = { 0.5f };		    // 각 점의 수명
    _float      fMinDistance = { 0.01f };	    // 새 점을 추가할 때 최소 거리
    _float      fTexRotation = {};			    // 텍스쳐 회전 수치(라디안)
    _float      fDistortionStrength = {};       // 왜곡 수치

    // Mesh Effect 데이터 (iEffectType = 2일 경우 사용)
    _string     strModelName = {};
    _int        iMeshRotateRight = {};
    _int        iMeshRotateUp = {};
    _int        iMeshRotateLook = {};
    _float3     vMeshPosition = {};
    _float3     vMeshScale = { 1.f, 1.f, 1.f };
    _float      fMeshScale = {};
    _float      fScaleDelay = {};
    _uint       iMeshScaleAxis = { 7 };
    _uint       iMeshMaskStartFrame = {};
    _uint       iMeshMaskFrameCount = { 1 };
    _uint       iMeshMaskCountX = { 1 };
    _uint       iMeshMaskCountY = { 1 };
    _float      fDisplaceStrength = {};
    _float2     vMeshMaskUVScroll = {};
    _float2     vMeshMaskUVScale = { 1.f, 1.f };
    _float2     vMeshFadeInOut = {};
    _float2     vMeshMaskUVStartOffset = {};
    _bool       bDirectionalDissolve = {};
    _bool       bDissolveFlipY = {};
    _float      fNoiseWeight = {};
    _float      fDissolveEdgeWidth = {};
    _float3     vDissolveEdgeColor = { 1.f, 1.f, 1.f };

    _bool       bAlignToPlayerLook = { false };

    // SubEmitter 목록
    vector<SubEmitterInfo>  vecSubEmitters;

    ordered_json To_Json() const
    {
        ordered_json j;
        j["ParticleEffectName"] = wstringToString(wstrEffectName);
        j["EffectType"] = eEffectType;
        j["StartDelay"] = fStartDelay;
        j["EffectLifeTime"] = fEffectLifeTime;

        // ParticleEffect 데이터
        j["EffectRotation"] = { vEffectRotation.x, vEffectRotation.y, vEffectRotation.z };
        j["TextureName"] = strTextureName;
        j["DiffuseUVScroll"] = { vDiffuseUVScroll.x, vDiffuseUVScroll.y };
        j["FadeInOut"] = { vFadeInOut.x, vFadeInOut.y };
        j["NoiseTextureName"] = strNoiseTextureName;
        j["MaskTextureName"] = strMaskTextureName;
        j["AlphaMaskTextureName"] = strAlphaMaskTextureName;
        j["ShaderTag"] = strShaderTag;
        j["ComputeShaderTag"] = strComputeShaderTag;
        j["PassIndex"] = iPassIndex;
        j["MaskSampler"] = iMaskSampler;
        j["UseEdgeGlow"] = bUseEdgeGlow;
        j["BackCulling"] = bBackCulling;
        j["Emissive"] = bEmissive;
        j["EffectLoop"] = bEffectLoop;
        j["GlowStrength"] = fGlowStrength;
        j["MaskIntensity"] = fMaskIntensity;
        j["Color"] = { vColor.x, vColor.y, vColor.z, vColor.w };
        j["HotColorFloat"] = fHotColor;
        j["HotColorVector"] = { vHotColor.x, vHotColor.y, vHotColor.z };
        j["EffectOffset"] = { vEffectOffset.x, vEffectOffset.y };

        // PARTICLE_DESC 데이터
        j["Loop"] = IsLoop;
        j["Billboard"] = IsBillboard;
        j["VerticalFade"] = IsVerticalFade;
        j["VerticalShrink"] = IsVerticalShrink;
        j["RadialRotation"] = IsRadialRotation;
        j["NumInstance"] = iNumInstance;
        j["Size"] = { vSize.x, vSize.y };
        j["Speed"] = { vSpeed.x, vSpeed.y };
        j["LifeTime"] = { vLifeTime.x, vLifeTime.y };
        j["vStartDelay"] = { vStartDelay.x, vStartDelay.y };
        j["Rotation"] = { vRotation.x, vRotation.y };
        j["Center"] = { vCenter.x, vCenter.y, vCenter.z };
        j["Range"] = { vRange.x, vRange.y, vRange.z };
        j["Pivot"] = { vPivot.x, vPivot.y, vPivot.z };
        j["Gravity"] = fGravity;
        j["Alpha"] = fAlpha;
        j["FrameCount"] = iFrameCount;
        j["FrameCountX"] = iCountX;
        j["FrameCountY"] = iCountY;
        j["TransparentIndex"] = iTransparentIndex;
        j["MaskStartFrame"] = iMaskStartFrame;
        j["MaskFrameCount"] = iMaskFrameCount;
        j["MaskFrameCountX"] = iMaskCountX;
        j["MaskFrameCountY"] = iMaskCountY;
        j["MaskUVScroll"] = { vMaskUVScroll.x, vMaskUVScroll.y };
        j["ScaleRatio"] = fParticleScale;
        j["DirectionalScale"] = iDirectionalScale;
        j["ShakeStrength"] = fShakeStrength;
        j["ConvergeMaxDist"] = fConvergeMaxDist;
        j["DistortionStrength"] = fDistortionStrength;

        // Trail Effect 데이터 (iEffectType = 1일 경우 사용)
        j["MaxPoints"] = iMaxPoints;
        j["TrailLifeTime"] = fLifeTime;
        j["MinDistance"] = fMinDistance;
        j["TexRotation"] = fTexRotation;

        // Mesh Effect 데이터 (iEffectType == 2)
        j["ModelName"] = strModelName;
        j["MeshRotateRight"] = iMeshRotateRight;
        j["MeshRotateUp"] = iMeshRotateUp;
        j["MeshRotateLook"] = iMeshRotateLook;
        j["MeshPosition"] = { vMeshPosition.x, vMeshPosition.y, vMeshPosition.z };
        j["MeshScale"] = { vMeshScale.x, vMeshScale.y, vMeshScale.z };
        j["MeshScaleRatio"] = fMeshScale;
        j["MeshScaleDelay"] = fScaleDelay;
        j["MeshScaleAxis"] = iMeshScaleAxis;
        j["MeshMaskStartFrame"] = iMeshMaskStartFrame;
        j["MeshMaskFrameCount"] = iMeshMaskFrameCount;
        j["MeshMaskCountX"] = iMeshMaskCountX;
        j["MeshMaskCountY"] = iMeshMaskCountY;
        j["DisplaceStrength"] = fDisplaceStrength;
        j["MeshMaskUVScroll"] = { vMeshMaskUVScroll.x, vMeshMaskUVScroll.y };
        j["MeshMaskUVScale"] = { vMeshMaskUVScale.x, vMeshMaskUVScale.y };
        j["MeshFadeInOut"] = { vMeshFadeInOut.x, vMeshFadeInOut.y };
        j["MeshMaskUVStartOffset"] = { vMeshMaskUVStartOffset.x, vMeshMaskUVStartOffset.y };

        j["DirectionalDissolve"] = bDirectionalDissolve;
        j["DissolveFlipY"] = bDissolveFlipY;
        j["NoiseWeight"] = fNoiseWeight;
        j["DissolveEdgeWidth"] = fDissolveEdgeWidth;
        j["DissolveEdgeColor"] = { vDissolveEdgeColor.x, vDissolveEdgeColor.y, vDissolveEdgeColor.z };

        j["AlignPlayerLook"] = bAlignToPlayerLook;

        // SubEmitter
        j["SubEmitters"] = ordered_json::array();
        for (const auto& SubEmitter : vecSubEmitters)
            j["SubEmitters"].push_back(SubEmitter.To_Json());

        return j;
    }

    void From_Json(const ordered_json& j)
    {
        wstrEffectName = stringToWstring(j.value("ParticleEffectName", ""));
        eEffectType = j.value("EffectType", (EFFECTTYPE)0);
        fStartDelay = j.value("StartDelay", 0.f);
        fEffectLifeTime = j.value("EffectLifeTime", 0.f);

        // ParticleEffect 데이터
        strTextureName = j.value("TextureName", "");
        strNoiseTextureName = j.value("NoiseTextureName", "");
        strMaskTextureName = j.value("MaskTextureName", "");
        strAlphaMaskTextureName = j.value("AlphaMaskTextureName", "");
        strShaderTag = j.value("ShaderTag", "");
        strComputeShaderTag = j.value("ComputeShaderTag", "");
        iPassIndex = j.value("PassIndex", 0);
        iMaskSampler = j.value("MaskSampler", 0);
        bUseEdgeGlow = j.value("UseEdgeGlow", false);
        bBackCulling = j.value("BackCulling", false);
        bEffectLoop = j.value("EffectLoop", false);
        bEmissive = j.value("Emissive", false);
        fGlowStrength = j.value("GlowStrength", 0.f);
        fMaskIntensity = j.value("MaskIntensity", 1.f);
        fHotColor = j.value("HotColorFloat", 0.f);

        if (j.contains("EffectRotation"))
            vEffectRotation = _float3(j["EffectRotation"][0], j["EffectRotation"][1], j["EffectRotation"][2]);

        if (j.contains("DiffuseUVScroll"))
            vDiffuseUVScroll = _float2(j["DiffuseUVScroll"][0], j["DiffuseUVScroll"][1]);

        if (j.contains("FadeInOut"))
            vFadeInOut = _float2(j["FadeInOut"][0], j["FadeInOut"][1]);

        if (j.contains("Color"))
            vColor = _float4(j["Color"][0], j["Color"][1], j["Color"][2], j["Color"][3]);

        if (j.contains("HotColorVector"))
            vHotColor = _float3(j["HotColorVector"][0], j["HotColorVector"][1], j["HotColorVector"][2]);

        if (j.contains("EffectOffset"))
            vEffectOffset = _float2(j["EffectOffset"][0], j["EffectOffset"][1]);

        // PARTICLE_DESC 데이터
        IsLoop = j.value("Loop", false);
        IsBillboard = j.value("Billboard", true);
        IsVerticalFade = j.value("VerticalFade", false);
        IsVerticalShrink = j.value("VerticalShrink", false);
        IsRadialRotation = j.value("RadialRotation", false);
        iNumInstance = j.value("NumInstance", 0);

        if (j.contains("Size"))
            vSize = _float2(j["Size"][0], j["Size"][1]);

        if (j.contains("Speed"))
            vSpeed = _float2(j["Speed"][0], j["Speed"][1]);

        if (j.contains("LifeTime"))
            vLifeTime = _float2(j["LifeTime"][0], j["LifeTime"][1]);

        if (j.contains("vStartDelay"))
            vStartDelay = _float2(j["vStartDelay"][0], j["vStartDelay"][1]);

        if (j.contains("Rotation"))
            vRotation = _float2(j["Rotation"][0], j["Rotation"][1]);

        if (j.contains("Center"))
            vCenter = _float3(j["Center"][0], j["Center"][1], j["Center"][2]);

        if (j.contains("Range"))
            vRange = _float3(j["Range"][0], j["Range"][1], j["Range"][2]);

        if (j.contains("Pivot"))
            vPivot = _float3(j["Pivot"][0], j["Pivot"][1], j["Pivot"][2]);

        fGravity = j.value("Gravity", 0.f);
        fAlpha = j.value("Alpha", 1.f);
        iFrameCount = j.value("FrameCount", 1);
        iCountX = j.value("FrameCountX", 1);
        iCountY = j.value("FrameCountY", 1);
        iTransparentIndex = j.value("TransparentIndex", 0);
        iMaskStartFrame = j.value("MaskStartFrame", 0);
        iMaskFrameCount = j.value("MaskFrameCount", 1);
        iMaskCountX = j.value("MaskFrameCountX", 1);
        iMaskCountY = j.value("MaskFrameCountY", 1);
        if (j.contains("MaskUVScroll"))
            vMaskUVScroll = _float2(j["MaskUVScroll"][0], j["MaskUVScroll"][1]);
        fParticleScale = j.value("ScaleRatio", 1.f);
        iDirectionalScale = j.value("DirectionalScale", 0);
        fShakeStrength = j.value("ShakeStrength", 1.f);
        fConvergeMaxDist = j.value("ConvergeMaxDist", 0.f);
        fDistortionStrength = j.value("DistortionStrength", 0.f);

        // Trail Effect 데이터 (iEffectType = 1일 경우 사용)
        iMaxPoints = j.value("MaxPoints", 64);
        fLifeTime = j.value("TrailLifeTime", 0.5f);
        fMinDistance = j.value("MinDistance", 0.01f);
        fTexRotation = j.value("TexRotation", 0.f);

        // Mesh Effect 데이터 (iEffectType == 2)
        strModelName = j.value("ModelName", "");
        iMeshRotateRight = j.value("MeshRotateRight", 0);
        iMeshRotateUp = j.value("MeshRotateUp", 0);
        iMeshRotateLook = j.value("MeshRotateLook", 0);
        fMeshScale = j.value("MeshScaleRatio", 1.f);
        fScaleDelay = j.value("MeshScaleDelay", 0.f);
        iMeshScaleAxis = j.value("MeshScaleAxis", 7);

        if (j.contains("MeshPosition"))
            vMeshPosition = _float3(j["MeshPosition"][0], j["MeshPosition"][1], j["MeshPosition"][2]);

        if (j.contains("MeshScale"))
            vMeshScale = _float3(j["MeshScale"][0], j["MeshScale"][1], j["MeshScale"][2]);

        iMeshMaskStartFrame = j.value("MeshMaskStartFrame", 0);
        iMeshMaskFrameCount = j.value("MeshMaskFrameCount", 1);
        iMeshMaskCountX = j.value("MeshMaskCountX", 1);
        iMeshMaskCountY = j.value("MeshMaskCountY", 1);
        fDisplaceStrength = j.value("DisplaceStrength", 0.f);

        if (j.contains("MeshMaskUVScroll"))
            vMeshMaskUVScroll = _float2(j["MeshMaskUVScroll"][0], j["MeshMaskUVScroll"][1]);

        if (j.contains("MeshMaskUVScale"))
            vMeshMaskUVScale = _float2(j["MeshMaskUVScale"][0], j["MeshMaskUVScale"][1]);

        if (j.contains("MeshFadeInOut"))
            vMeshFadeInOut = _float2(j["MeshFadeInOut"][0], j["MeshFadeInOut"][1]);

        if (j.contains("MeshMaskUVStartOffset"))
            vMeshMaskUVStartOffset = _float2(j["MeshMaskUVStartOffset"][0], j["MeshMaskUVStartOffset"][1]);

        bDirectionalDissolve = j.value("DirectionalDissolve", false);
        bDissolveFlipY = j.value("DissolveFlipY", false);
        fNoiseWeight = j.value("NoiseWeight", 0.f);
        fDissolveEdgeWidth = j.value("DissolveEdgeWidth", 0.f);

        if (j.contains("DissolveEdgeColor"))
            vDissolveEdgeColor = _float3(j["DissolveEdgeColor"][0], j["DissolveEdgeColor"][1], j["DissolveEdgeColor"][2]);

        bAlignToPlayerLook = j.value("AlignPlayerLook", false);

        vecSubEmitters.clear();
        if (j.contains("SubEmitters") && j["SubEmitters"].is_array())
        {
            for (const auto& SubEmitter : j["SubEmitters"])
            {
                SubEmitterInfo subEmitterInfo;
                subEmitterInfo.From_Json(SubEmitter);
                vecSubEmitters.push_back(subEmitterInfo);
            }
        }
    }
};

// ParticlySystem 하나의 데이터
struct ParticleSystemInfo
{
    _wstring                    wstrSystemName = {};
    _bool                       bIsAutoPlay = {};
    _bool                       bIsLoop = {};
    _float                      fTotalDuration = {};
    _float                      fRotationSpeed = { 1.f };
    _float                      fDelayedTime = {};
    _float3                     vRotationSpeed = {};
    vector<ParticleEffectInfo>  vecEffects;

    ordered_json To_Json() const
    {
        ordered_json j;
        j["SystemName"] = wstringToString(wstrSystemName);
        j["AutoPlay"] = bIsAutoPlay;
        j["Loop"] = bIsLoop;
        j["TotalDuration"] = fTotalDuration;
        j["DelayedTime"] = fDelayedTime;
        j["RotationSpeed"] = { vRotationSpeed.x, vRotationSpeed.y, vRotationSpeed.z };

        j["Effects"] = ordered_json::array();
        for (const auto& effect : vecEffects)
            j["Effects"].push_back(effect.To_Json());

        return j;
    }

    void From_Json(const ordered_json& j)
    {
        wstrSystemName = stringToWstring(j.value("SystemName", ""));
        bIsAutoPlay = j.value("AutoPlay", false);
        bIsLoop = j.value("Loop", false);
        fTotalDuration = j.value("TotalDuration", 0.f);
        fDelayedTime = j.value("DelayedTime", 0.f);

        if (j.contains("RotationSpeed"))
            vRotationSpeed = _float3(j["RotationSpeed"][0], j["RotationSpeed"][1], j["RotationSpeed"][2]);

        vecEffects.clear();
        if (j.contains("Effects") && j["Effects"].is_array())
        {
            for (const auto& effectInfo : j["Effects"])
            {
                ParticleEffectInfo info;
                info.From_Json(effectInfo);
                vecEffects.push_back(info);
            }
        }
    }
};

// ========== UI툴용 정보 ==========
// GPT기준 예시 변수들로 적어둠
struct UIObjectInfo
{
    ObjectKey                   strObjectKey = "";
    // 오브젝트 고유 키 (매칭용)
    string                      UIBaseType = "NONE";
    string                      UIType = "DEFAULT";

    _int                        iZOrder = 0;
    string                      strParentName = "";
    string                      ShaderComName = "";

    wstring                     UIDataStr = L"";


    bool            bVisible = true;
    bool            bActive = true;
    bool            bIsInteratable = true;

    bool            m_bMaskProgress = true;
    bool            bIsBlurUI = false;
    _uint           iShaderPass = 4;

    _float          m_fValue = 0.f;
    _float          m_fIntensity = 1.f;

    RENDER_GROUP        m_eRenderGroup = RENDER_GROUP::UI;

    //UITransform Data
    UITransform     m_Local;                //로컬좌표 저장
    // 렌더 순서

//히트박스  패딩
    _float2      m_HitBoxPadding = _float2(0.f, 0.f);
    _float2      m_HitBoxSize = _float2(0.f, 0.f);

    //이벤트
    UI_ActiveEvent  m_ActiveEvent[2];


    vector<ComponentData>       vecComponents;


    vector<UIObjectInfo>        m_Children;

    ordered_json To_Json() const
    {
        ordered_json j;
        //1.기본정보
        j["ObjectKey"] = strObjectKey;

        j["BaseType"] = UIBaseType;
        j["UIType"] = UIType;

        j["ZOrder"] = iZOrder;
        j["Parent"] = strParentName;
        j["ShaderName"] = ShaderComName;
        j["BlurUI"] = bIsBlurUI;
        j["MaskProgress"] = m_bMaskProgress;
        j["UIDataStr"] = wstringToString(UIDataStr);

        //2.상태값저장
        j["Visible"] = bVisible;
        j["Active"] = bActive;
        j["IsInteratable"] = bIsInteratable;
        j["ShaderPass"] = iShaderPass;
        j["Value"] = m_fValue;
        j["Intensity"] = m_fIntensity;
        
        string strRenderGroup = "";
        switch (m_eRenderGroup)
        {

        case Engine::RENDER_GROUP::UI:
            strRenderGroup = "RENDERGROUP::UI";
            break;

            break;
        case Engine::RENDER_GROUP::WORLD_UI:
            strRenderGroup = "RENDERGROUP::WORLD_UI";
            break;

        default:
            break;
        }
        j["RenderGroup"] = strRenderGroup;

        //3.Transform 저장
        j["Transform"] = m_Local.To_Json();

        //히트박스 패딩저장
        j["HitBox_Padding"] = { m_HitBoxPadding.x,m_HitBoxPadding.y };
        j["HitBox_Size"] = { m_HitBoxSize.x,m_HitBoxSize.y };

        //4.이벤트저장
        j["ActiveEvents"]["OnActive"] = m_ActiveEvent[0].To_Json();
        j["ActiveEvents"]["OnDisable"] = m_ActiveEvent[1].To_Json();

        //5.컴포넌트저장
        j["Components"] = ordered_json::array();
        for (auto& pComp : vecComponents)
        {
            ordered_json compJson = pComp.To_Json();
            j["Components"].push_back(compJson);
        }

        //6.자식정보
        j["Children"] = ordered_json::array();
        for (auto& pChild : m_Children)
            j["Children"].push_back(pChild.To_Json());

        return j;
    }

    void From_Json(const ordered_json& j)
    {
        //1.기본정보
        strObjectKey = j.value("ObjectKey", "");
        UIType = j.value("UIType", "DEFAULT");

        UIBaseType = (j.value("BaseType", "NONE"));
        UIType = (j.value("UIType", "DEFAULT"));

        bIsBlurUI = (j.value("BlurUI", false));
        m_bMaskProgress = (j.value("MaskProgress", true));
     
        UIDataStr = stringToWstring(j.value("UIDataStr", ""));
        strParentName = j.value("Parent", "");
        iZOrder = j.value("ZOrder", 0);
        ShaderComName = j.value("ShaderName", "Prototype_Component_Shader_VTXPosTex");
        m_fValue = j.value("Value", 0.f);
        m_fIntensity = j.value("Intensity", 1.f);


        //2.상태값
        bVisible = j.value("Visible", true);
        bActive = j.value("Active", true);
        bIsInteratable = j.value("IsInteratable", true);
        iShaderPass = j.value("ShaderPass", 4);

        string strRenderGroup = j.value("RenderGroup", "RENDERGROUP::UI");
        if (strRenderGroup == "RENDERGROUP::UI")
            m_eRenderGroup = RENDER_GROUP::UI;

        else if (strRenderGroup == "RENDERGROUP::WORLD_UI")
            m_eRenderGroup = RENDER_GROUP::WORLD_UI;

        //3. Transform
        if (j.contains("Transform"))
            m_Local.From_Json(j["Transform"]);

        if (j.contains("HitBox_Padding"))
            m_HitBoxPadding = _float2(j["HitBox_Padding"][0], j["HitBox_Padding"][1]);

        if (j.contains("HitBox_Size"))
            m_HitBoxSize = _float2(j["HitBox_Size"][0], j["HitBox_Size"][1]);

        else
            m_HitBoxSize = _float2(m_Local.m_fSizeX, m_Local.m_fSizeY);


        //4.이벤트
        if (j.contains("ActiveEvents"))
        {
            auto& events = j["ActiveEvents"];
            if (events.contains("OnActive")) m_ActiveEvent[0].From_Json(events["OnActive"]);
            if (events.contains("OnDisable")) m_ActiveEvent[1].From_Json(events["OnDisable"]);
        }

        //5.컴포넌트
        vecComponents.clear();
        if (j.contains("Components") && j["Components"].is_array())
        {
            for (auto& ComJson : j["Components"])
            {
                ComponentData Data;
                Data.From_Json(ComJson);
                vecComponents.push_back(Data);
            }
        }

        //6.자식로드
        m_Children.clear();
        if (j.contains("Children") && j["Children"].is_array())
        {
            for (auto& ChildrenData : j["Children"])
            {
                UIObjectInfo UIInfo;
                UIInfo.From_Json(ChildrenData);
                m_Children.push_back(UIInfo);
            }
        }
    }
};

/*ui poolingfunc*/
using UIPOOLINGFUNC = std::function < class UIObject* (UIObjectInfo& pInfo) >;



// ========== 애니메이션툴용 정보 ==========
// GPT기준 예시 변수들로 적어둠
struct AnimationObjectInfo
{
    ObjectKey       strObjectKey = "";            // 오브젝트 고유 키 (매칭용)
    _wstring        wstrDefaultAnimation = L"";   // 기본 애니메이션
    _float3         vScale = { 1.f, 1.f, 1.f };     // 캐릭터의 크기
    _float          fSpeed = { 10.f };            // 캐릭터의 속도
    _float          fRotationSpeed = { 10.f };    // 캐릭터의 회전속도
    _bool           bLoop = true;                 // 루프 여부

    ordered_json To_Json() const
    {
        ordered_json j;
        j["ObjectKey"] = strObjectKey;
        j["DefaultAnimation"] = wstringToString(wstrDefaultAnimation);
        j["Scale"] = vector<_float>(
            reinterpret_cast<const _float*>(&vScale),
            reinterpret_cast<const _float*>(&vScale) + 3
        );;
        j["Loop"] = bLoop;
        j["Speed"] = fSpeed;
        j["fRotationSpeed"] = fRotationSpeed;
        return j;
    }

    void From_Json(const ordered_json& j)
    {
        strObjectKey = j.value("ObjectKey", "");
        wstrDefaultAnimation = stringToWstring(j.value("DefaultAnimation", ""));
        if (j.contains("Scale"))
        {
            auto arr = j["Scale"].get<vector<_float>>();
            if (arr.size() == 3)
                memcpy(&vScale, arr.data(), sizeof(_float3));
        }
        bLoop = j.value("Loop", true);
        fSpeed = j.value("Speed", 10.f);
        fRotationSpeed = j.value("fRotationSpeed", 10.f);
    }
};


// ========== 클라이언트 로드 결과 ==========
// 여기에 결과 담아서 한번에 객체 생성함
struct LevelLoadResult
{
    _bool bReadMap = false;
    _bool bReadShader = false;
    _bool bReadEffect = false;
    _bool bReadAnimation = false;
    _bool bReadUI = false;

    // 맵 오브젝트들 (인스턴스 목록 - 같은 키 여러 개 가능)
    vector<MapObjectInfo> vecMapObjects = {};

    // 프로토타입별 설정 (룩업 테이블 - 키당 1개)
    UMAP<ObjectKey, ShaderObjectInfo> umapShaderInfos = {};
    UMAP<ObjectKey, ParticleEffectInfo> umapEffectInfos = {};
    UMAP<ObjectKey, AnimationObjectInfo> umapAnimInfos = {};

    // UI는 별도 (맵 오브젝트와 무관하게 독립적)
    vector<UIObjectInfo> vecUIObjects = {};
};

NS_END


